#include "debug_log.h"

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <functional>
#include <mutex>
#include <thread>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__GLIBC__)
#include <csignal>
#include <execinfo.h>
#include <unistd.h>
#endif

using namespace godot;

namespace {
bool g_enabled = false;
FILE *g_file = nullptr;
std::mutex g_mutex;
std::chrono::steady_clock::time_point g_start;

// A crash in a thread of our own (the engine, its timer, the music) does not reach Godot's crash
// handler, which guards only the main thread, and the game just ends. The crash is written to
// stderr and with --debug-log also to the log: what it was, where (module + offset, for the .pdb /
// addr2line) and the stack. Nothing is changed in how the crash is handled.

// written without the mutex: the crashing thread may hold it
void CrashLine(const char *text) {
	fputs(text, stderr);
	fflush(stderr);
	if (g_file) {
		fputs(text, g_file);
		fflush(g_file);
	}
}

#ifdef _WIN32
void CrashAddress(int frame, DWORD64 address) {
	char line[600];
	HMODULE module = nullptr;
	char name[MAX_PATH] = "?";
	if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				(LPCSTR)address, &module) &&
			GetModuleFileNameA(module, name, MAX_PATH)) {
		const char *base = strrchr(name, '\\');
		snprintf(line, sizeof(line), "[mb crash]   #%02d %s+0x%llx\n", frame, base ? base + 1 : name,
				(unsigned long long)(address - (DWORD64)module));
	} else
		snprintf(line, sizeof(line), "[mb crash]   #%02d 0x%llx\n", frame, (unsigned long long)address);
	CrashLine(line);
}

LPTOP_LEVEL_EXCEPTION_FILTER g_previous = nullptr;

// an exception nobody handled, in any thread - the game ends
LONG WINAPI CrashHandler(EXCEPTION_POINTERS *info) {
	const DWORD code = info->ExceptionRecord->ExceptionCode;
	char line[300];
	const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - g_start).count();
	snprintf(line, sizeof(line), "[mb %9.3f t%04x] CRASH exception 0x%08lx", seconds,
			(unsigned)(std::hash<std::thread::id>{}(std::this_thread::get_id()) & 0xFFFF), code);
	CrashLine(line);
	if (code == EXCEPTION_ACCESS_VIOLATION && info->ExceptionRecord->NumberParameters >= 2)
		snprintf(line, sizeof(line), " (%s 0x%llx)\n", info->ExceptionRecord->ExceptionInformation[0] == 1 ? "write" : "read",
				(unsigned long long)info->ExceptionRecord->ExceptionInformation[1]);
	else
		snprintf(line, sizeof(line), "\n");
	CrashLine(line);
#ifdef _M_X64
	// the stack of the crashed thread, unwound from its context
	CONTEXT context = *info->ContextRecord;
	for (int frame = 0; frame < 40 && context.Rip; frame++) {
		CrashAddress(frame, context.Rip);
		DWORD64 image_base = 0;
		PRUNTIME_FUNCTION function = RtlLookupFunctionEntry(context.Rip, &image_base, nullptr);
		if (!function) { // a leaf function: the return address is on the top of the stack
			context.Rip = *(DWORD64 *)context.Rsp;
			context.Rsp += 8;
			continue;
		}
		void *handler_data = nullptr;
		DWORD64 establisher = 0;
		RtlVirtualUnwind(UNW_FLAG_NHANDLER, image_base, context.Rip, function, &context, &handler_data, &establisher, nullptr);
	}
#else
	CrashAddress(0, (DWORD64)info->ExceptionRecord->ExceptionAddress);
#endif
	return g_previous ? g_previous(info) : EXCEPTION_CONTINUE_SEARCH;
}

void CrashHandlerInstall() {
	g_previous = SetUnhandledExceptionFilter(CrashHandler);
}

void CrashHandlerRemove() {
	SetUnhandledExceptionFilter(g_previous);
}
#elif defined(__GLIBC__)
struct sigaction g_previous[32];

void CrashHandler(int signal) {
	char line[200];
	const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - g_start).count();
	snprintf(line, sizeof(line), "[mb %9.3f] CRASH signal %d (%s)\n", seconds, signal, strsignal(signal));
	CrashLine(line);
	void *frames[64];
	const int count = backtrace(frames, 64);
	backtrace_symbols_fd(frames, count, STDERR_FILENO);
	if (g_file)
		backtrace_symbols_fd(frames, count, fileno(g_file));
	// on to the handler before ours (Godot's backtrace) or the default action
	sigaction(signal, &g_previous[signal], nullptr);
	raise(signal);
}

constexpr int kCrashSignals[] = { SIGSEGV, SIGABRT, SIGBUS, SIGFPE, SIGILL };

void CrashHandlerInstall() {
	for (int signal : kCrashSignals) {
		struct sigaction action = {};
		action.sa_handler = CrashHandler;
		sigaction(signal, &action, &g_previous[signal]);
	}
}

void CrashHandlerRemove() {
	for (int signal : kCrashSignals)
		sigaction(signal, &g_previous[signal], nullptr);
}
#else // Android, macOS: Godot's own handler only
void CrashHandlerInstall() {}
void CrashHandlerRemove() {}
#endif
} // namespace

void MBDebugLogInit() {
	g_start = std::chrono::steady_clock::now();
	// always: a crash is written to stderr (and to debug.log with --debug-log)
	CrashHandlerInstall();
	OS *os = OS::get_singleton();
	if (!os || !os->get_cmdline_user_args().has("--debug-log"))
		return;
	g_enabled = true;
	const String path = os->get_user_data_dir().path_join("debug.log");
#ifdef _WIN32
	g_file = _wfopen((const wchar_t *)path.wide_string().get_data(), L"w");
#else
	g_file = fopen(path.utf8().get_data(), "w");
#endif
	// the wall clock of the log's 0 s - to match the log with outside timings (when the process ended)
	const auto now = std::chrono::system_clock::now();
	const std::time_t seconds = std::chrono::system_clock::to_time_t(now);
	const int ms = (int)(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000);
	char clock[32];
	std::strftime(clock, sizeof(clock), "%H:%M:%S", std::localtime(&seconds));
	MBDebugLog("debug log on at %s.%03d, file %s", clock, ms, g_file ? path.utf8().get_data() : "(could not be opened)");
}

// the handler's code goes away with the library: removed when it is unloaded, as late as possible
// (a crash in the library's static destructors is still caught)
struct CrashHandlerGuard {
	~CrashHandlerGuard() { CrashHandlerRemove(); }
} g_crash_handler_guard;

bool MBDebugLogEnabled() {
	return g_enabled;
}

void MBDebugLog(const char *format, ...) {
	if (!g_enabled)
		return;
	char text[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(text, sizeof(text), format, args);
	va_end(args);
	const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - g_start).count();
	const unsigned thread = (unsigned)(std::hash<std::thread::id>{}(std::this_thread::get_id()) & 0xFFFF);
	std::lock_guard<std::mutex> lock(g_mutex);
	fprintf(stderr, "[mb %9.3f t%04x] %s\n", seconds, thread, text);
	fflush(stderr);
	if (g_file) {
		fprintf(g_file, "[mb %9.3f t%04x] %s\n", seconds, thread, text);
		fflush(g_file);
	}
}

void MBDebug::_bind_methods() {
	ClassDB::bind_static_method("MBDebug", D_METHOD("log", "text"), &MBDebug::log);
	ClassDB::bind_static_method("MBDebug", D_METHOD("enabled"), &MBDebug::enabled);
}

void MBDebug::log(const String &text) {
	if (g_enabled)
		MBDebugLog("%s", text.utf8().get_data());
}

bool MBDebug::enabled() {
	return g_enabled;
}

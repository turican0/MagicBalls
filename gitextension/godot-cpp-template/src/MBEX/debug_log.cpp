#include "debug_log.h"

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <functional>
#include <mutex>
#include <thread>

using namespace godot;

namespace {
bool g_enabled = false;
FILE *g_file = nullptr;
std::mutex g_mutex;
std::chrono::steady_clock::time_point g_start;
} // namespace

void MBDebugLogInit() {
	OS *os = OS::get_singleton();
	if (!os || !os->get_cmdline_user_args().has("--debug-log"))
		return;
	g_enabled = true;
	g_start = std::chrono::steady_clock::now();
	const String path = os->get_user_data_dir().path_join("debug.log");
#ifdef _WIN32
	g_file = _wfopen((const wchar_t *)path.wide_string().get_data(), L"w");
#else
	g_file = fopen(path.utf8().get_data(), "w");
#endif
	MBDebugLog("debug log on, file %s", g_file ? path.utf8().get_data() : "(could not be opened)");
}

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

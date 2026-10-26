#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

// A log to find where the game stops or crashes, on with the user argument --debug-log
// (MagicBalls -- --debug-log). Every line goes at once to stderr and to debug.log in the user data
// folder, flushed: print() writes to stdout, which an exported game buffers and loses in a crash.
// A line has the seconds since the log started and the thread it was written from.

void MBDebugLogInit(); // reads the command line - when the extension is initialized
bool MBDebugLogEnabled();
void MBDebugLog(const char *format, ...);

// MBDebug.log(text) / MBDebug.enabled() for GDScript, into the same log
class MBDebug : public godot::RefCounted {
	GDCLASS(MBDebug, godot::RefCounted)

protected:
	static void _bind_methods();

public:
	static void log(const godot::String &text);
	static bool enabled();
};

#pragma once

#include <chrono>
#include <cstdint>

// Time spent in the parts of the engine in the current frame (microseconds). REMC2Run clears it at
// the start of a frame, Godot reads it with getFrameProfile() for the profiling (Global.profiling).
// The sections of the engine thread are written only while the Godot thread waits for it, so
// nothing is shared at the same time.
enum FrameProfileSection {
	PROF_ENGINE_WAIT, // REMC2Run waiting for the engine thread (= the whole engine step)
	PROF_SCREEN_IMAGE, // the 2D screen of the engine -> Godot texture
	PROF_PALETTE, // palette changes, sprite animation
	PROF_INPUT, // keys, mouse, player events
	PROF_ENTITIES, // UpdateEntities (the game logic)
	PROF_LIGHTING, // sub_84B80
	PROF_OBJECTIVES, // objectives, entity sounds
	PROF_DRAW_2D, // the 2D frame of the engine (UI, help, blit)
	PROF_MINIMAP, // the minimap - part of PROF_DRAW_2D
	PROF_TOP_BAR, // the spell icons and the status bar - part of PROF_DRAW_2D
	PROF_COUNT
};

extern int64_t frameProfileUsec[PROF_COUNT];

void FrameProfileClear();

// Counts the time into `section` until Switch() moves it to the next section or the scope ends.
struct FrameProfileScope {
	FrameProfileSection section;
	std::chrono::steady_clock::time_point start;
	explicit FrameProfileScope(FrameProfileSection s) :
			section(s), start(std::chrono::steady_clock::now()) {}
	void Switch(FrameProfileSection next) {
		const auto now = std::chrono::steady_clock::now();
		frameProfileUsec[section] += std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
		section = next;
		start = now;
	}
	~FrameProfileScope() {
		frameProfileUsec[section] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
	}
};

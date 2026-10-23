#include "../engine/engine_support.h"
#include "port_sdl_sound.h"

#include "../engine/PlayerInput.h"
#include "awe32/Awe32Mdi.h"
#include "awe32/Awe32Rom.h"

#include <iostream>
#include <list>
#include <mutex>
#include <vector>

// MagicBalls: the engine does not play any sound itself. Every request of the AIL layer is put
// into sound_queue as a SoundAction; Godot takes the queue once a frame (getPendingSoundActions)
// and plays the samples, the music and the speech with its own players (scenes/sounds.gd).
// Godot reports back which sample players still play (updateFreeSoundPlayers).
// The AWE32 music is the exception: the EMU8000 emulation renders PCM in its own thread and
// Godot pulls it into an AudioStreamGenerator (SOUND_RenderAwe32Music).

bool debug_first_sound = true;

bool hqsound = false;
bool oggmusic = false;
bool oggmusicalternative = false;
char oggmusicFolder[512];
char speechFolder[512];

bool fixspeedsound = false;
bool autoShowObjectivesForForeignLanguages = false;
int maxSimultaniousSounds = 10;

std::string musicType = "ogg";
std::string awe32RomFolder = "awe32data";
bool awe32Music = false;
bool useMagicCarpet1Music = false;
bool awe32GmIntroMusic = true;
int awe32BufferMs = 200;
static std::string awe32RomPath;
static std::string musicWarning;//shown in the game when the AWE32 music could not be used

int lastMusicVolume = -1;
int settingsMusicVolume = 127;
int num_IO_configurations = 3;
int service_rate = -1;
int master_volume = -1;

uint8_t sound_buffer[4][20000];

std::vector<SoundAction> sound_queue;

void sound_queue_add_action(const std::string &action, int p1, int p2, int p3, int p4) {
	SoundAction sa;
	sa.action = action;
	sa.p1 = p1;
	sa.p2 = p2;
	sa.p3 = p3;
	sa.p4 = p4;
	sound_queue.push_back(sa);
}

//----------------------------------------------------------------------------------------------
// Samples
//----------------------------------------------------------------------------------------------

// Godot reports which players play (updateFreeSoundPlayers) at the start of a frame, after it
// played the actions of the previous one. A start still waiting in sound_queue is not in that
// report yet, so it counts as playing; otherwise AIL_sample_status says "done" right after
// AIL_start_sample and the game starts the sound again (the repeating sounds).
static std::vector<int> playingSamples;
static std::map<int, HSAMPLE> channelSamples;
static std::map<int, uint32_t> sampleStartsQueued;
static std::map<int, uint32_t> sampleStartsDelivered;

static bool SampleStartPending(int channel) {
	return sampleStartsQueued[channel] != sampleStartsDelivered[channel];
}

static bool IsChannelPlaying(int channel) {
	if (channel < 0)
		return false;
	return SampleStartPending(channel) || (channel < (int)playingSamples.size() && playingSamples[channel] > 0);
}

static void ChannelFinished(int channel) {
	auto it = channelSamples.find(channel);
	if (it == channelSamples.end() || !it->second)
		return;
	HSAMPLE S = it->second;
	const bool wasPlaying = (S->status_1 == 4);//AilSampleStarted
	S->status_1 = 2;
	// The original AIL driver calls the EOS callback (sample+0x850) when the last buffer
	// finishes; the game's EOS handler (sub_9EE70) starts the next sample of a chain.
	// An explicit AIL_end_sample sets status 2 before halting and calls EOS itself.
	if (wasPlaying && S->sam_var532_EOS_ptr)
		S->sam_var532_EOS_ptr();
}

void sound_update_playing(const std::vector<int> &playing) {
	playingSamples = playing;
	// a started sample whose player is quiet has ended (Mix_ChannelFinished in remc2)
	std::vector<int> finished;
	for (const auto &[channel, S] : channelSamples)
		if (S && S->status_1 == 4 && !IsChannelPlaying(channel))
			finished.push_back(channel);
	for (int channel : finished)
		ChannelFinished(channel);
}

void SOUND_start_sample(HSAMPLE S) {
	if (unitTests || S->channel < 0)
		return;
	channelSamples[S->channel] = S;
	sampleStartsQueued[S->channel]++;
	sound_queue_add_action("SOUND_start_sample", S->channel, S->id2_19, S->volume_16, S->playback_rate_15);
}

void SOUND_end_sample(HSAMPLE S) {
	if (S->channel < 0)
		return;
	sound_queue_add_action("SOUND_end_sample", S->channel, 0, 0, 0);
	channelSamples.erase(S->channel);
}

uint32_t SOUND_sample_status(HSAMPLE S) {
	if (unitTests)
		return 0;
	//original AIL_sample_status returns status_1: 1=free, 2=done, 4=playing
	return IsChannelPlaying(S->channel) ? 4 : 2;
}

void SOUND_set_master_volume(int32_t volume) {
	master_volume = volume;
	sound_queue_add_action("SOUND_set_master_volume", volume, 0, 0, 0);
}

void SOUND_set_sample_volume(HSAMPLE S, int32_t volume) {
	sound_queue_add_action("SOUND_set_sample_volume", S->channel, volume, 0, 0);
}

void SOUND_set_sample_volume_panning(HSAMPLE S, int32_t panning) {
	sound_queue_add_action("SOUND_set_sample_volume_panning", S->channel, panning, 0, 0);
}

void SetSamplePosition(HSAMPLE S, int16_t angle, uint8_t distance) {
	sound_queue_add_action("SetSamplePosition", S->channel, angle, distance, 0);
}

//----------------------------------------------------------------------------------------------
// Music (ogg/MIDI in Godot)
//----------------------------------------------------------------------------------------------

// Godot loads the music itself (the MIDI files converted from the XMI, or the ogg tracks).
void SOUND_init_MIDI_sequence(uint8_t* /*datax*/, type_E3808_music_header* /*headerx*/, int32_t /*track_number*/) {
}

void SOUND_start_sequence(int32_t sequence_num) {
	if (unitTests)
		return;
	sound_queue_add_action("SOUND_start_sequence", sequence_num, 0, 0, 0);
	//volume fix
	if (lastMusicVolume == -1)
		SOUND_set_sequence_volume(0x64, 0);
	if (lastMusicVolume != settingsMusicVolume)
		SOUND_set_sequence_volume(settingsMusicVolume, 0);
}

void WarMusicSetVolume(int32_t volume) {
	sound_queue_add_action("SOUND_warmusic_setvolume", volume, 0, 0, 0);
}

void SOUND_pause_sequence(int32_t /*sequence_num*/) {
}

void SOUND_stop_sequence(int32_t /*sequence_num*/) {
	if (unitTests)
		return;
	sound_queue_add_action("SOUND_stop_sequence", 0, 0, 0, 0);
}

void SOUND_resume_sequence(int32_t /*sequence_num*/) {
}

void SOUND_set_sequence_volume(int32_t volume, int32_t milliseconds) {
	sound_queue_add_action("SOUND_set_sequence_volume", volume, milliseconds, 0, 0);
	lastMusicVolume = volume;
	if (milliseconds == 0)
		settingsMusicVolume = volume;
}

//----------------------------------------------------------------------------------------------
// Music (AWE32 emulation)
//----------------------------------------------------------------------------------------------

bool SOUND_IsAwe32Music() {
	return awe32Music;
}

void SOUND_RenderAwe32Music(int16_t* out, int frames) {
	Awe32Mdi::Render(out, frames);
}

void SOUND_ShowMusicWarning() {
	if (musicWarning.empty())
		return;
	SetCurrentNotificationMessage_19760(musicWarning.c_str(), 3u, 100);
	musicWarning.clear();
}

//DOS conventional memory (DPMI 0x100/0x101) for the real mode buffers the game hands over to the driver
static const uint16_t dosRealFirstSegment = 0x1000;
static const uint16_t dosRealLastSegment = 0xA000;
static std::vector<uint8_t> dosRealMemory;
static std::list<std::pair<uint16_t, uint16_t>> dosRealBlocks;//segment, paragraphs

uint16_t SOUND_DosRealAlloc(uint32_t bytes) {
	if (dosRealMemory.empty())
		dosRealMemory.resize((dosRealLastSegment - dosRealFirstSegment) * 16);
	const uint32_t paragraphs = (bytes + 15) >> 4;
	uint32_t segment = dosRealFirstSegment;
	auto it = dosRealBlocks.begin();
	for (; it != dosRealBlocks.end(); ++it) {
		if (segment + paragraphs <= it->first)
			break;
		segment = it->first + it->second;
	}
	if (paragraphs == 0 || segment + paragraphs > dosRealLastSegment)
		return 0;
	dosRealBlocks.insert(it, std::make_pair((uint16_t)segment, (uint16_t)paragraphs));
	memset(&dosRealMemory[(segment - dosRealFirstSegment) * 16], 0, paragraphs * 16);
	return (uint16_t)segment;
}

void SOUND_DosRealFree(uint16_t segment) {
	dosRealBlocks.remove_if([segment](const std::pair<uint16_t, uint16_t>& block) { return block.first == segment; });
}

uint8_t* SOUND_DosRealPointer(uint16_t segment, uint16_t offset) {
	for (const auto& block : dosRealBlocks)
		if (block.first == segment && offset < block.second * 16)
			return &dosRealMemory[(segment - dosRealFirstSegment) * 16 + offset];
	return nullptr;
}

//----------------------------------------------------------------------------------------------
// AIL driver
//----------------------------------------------------------------------------------------------

struct {
	int a;
} common_IO_configurations;

struct {
	int a;
} environment_string;

int32_t ac_sound_call_driver(AIL_DRIVER* drvr, int32_t fn, VDI_CALL* out) {
	if (awe32Music && drvr->type_5 == 1) {
		switch (fn) {
		case 0x300://install: the driver image read from the file
			Awe32Mdi::Install(drvr->buf_2, drvr->size_3, awe32RomPath);
			break;
		case 0x306://shutdown
			Awe32Mdi::Shutdown();
			return 1;
		case 0x501: {//init instrument manager
			std::string error;
			const bool ok = Awe32Mdi::InitInstruments(error);
			if (!ok)
				Logger->error("AWE32: {}", error);
			if (out)
				out->AX = ok ? 1 : 0;
			return 1;
		}
		case 0x502: {//send the buffered MIDI messages
			HMDIDRIVER mdi = (HMDIDRIVER)drvr->descriptor_10;
			if (mdi)
				Awe32Mdi::SendMessages((const uint8_t*)mdi->DST_2->MIDI_data, mdi->message_count);
			return 1;
		}
		case 0x503://install timbre bank: everything is in the ROM and the user bank
			if (out)
				out->AX = 1;
			return 1;
		case 0x506: {//AWE32 functions
			if (!out)
				return 1;
			Awe32Mdi::Regs regs = { out->AX, out->BX, out->CX, out->DX };
			Awe32Mdi::Extension(regs, SOUND_DosRealPointer, SOUND_DosRealAlloc);
			out->AX = regs.AX;
			out->CX = regs.CX;
			out->DX = regs.DX;
			return 1;
		}
		default:
			break;
		}
	}
	switch (fn) {
	case 0x300: {//AIL_API_install_driver
		drvr->VHDR_4->VDI_HDR_var10 = (void*)&common_IO_configurations;
		drvr->VHDR_4->num_IO_configurations_14 = num_IO_configurations;
		drvr->VHDR_4->environment_string_16 = environment_string.a;
		drvr->VHDR_4->VDI_HDR_var46 = service_rate;
		break;
	}
	case 0x301: {//AIL_API_install_DIG_driver_file/AIL_API_install_MDI_driver_file
		out->AX = 1;//offset
		out->BX = 2;//offset
		out->SI = 0;
		out->DI = 0;
		break;
	}
	default:
		break;
	}
	return 1;
}

uint16_t actvect[4096];

void ac_set_real_vect(uint32_t vectnum, uint16_t real_ptr) {
	actvect[vectnum] = real_ptr;
}

uint16_t ac_get_real_vect(uint32_t vectnum) {
	return actvect[vectnum];
}

//----------------------------------------------------------------------------------------------
// AIL timers - fired from the Godot thread (getPendingSoundActions -> sound_queue_clear)
//----------------------------------------------------------------------------------------------

struct SoundTimer {
	int Id;
	uint32_t IntervalMs;
	std::function<uint32_t(uint32_t)> Callback;
	bool Running{ false };
	std::chrono::steady_clock::time_point NextTick;
};
static std::list<SoundTimer> Timers;
// Timers are registered/stopped by the engine and fired from the Godot thread.
// Recursive, because a callback may stop or restart a timer itself.
static std::recursive_mutex TimersMutex;

static std::list<SoundTimer>::iterator FindTimer(int timerIdx) {
	for (auto it = Timers.begin(); it != Timers.end(); ++it)
		if (it->Id == timerIdx)
			return it;
	return Timers.end();
}

void SOUND_RegisterTimer(int timerIdx, uint32_t (*callback)(uint32_t)) {
	std::lock_guard<std::recursive_mutex> lock(TimersMutex);
	Timers.emplace_back();
	Timers.back().Id = timerIdx;
	Timers.back().Callback = callback;
}

void SOUND_SetTimerPeriod(int timerIdx, uint32_t intervalMs) {
	std::lock_guard<std::recursive_mutex> lock(TimersMutex);
	auto it = FindTimer(timerIdx);
	if (it != Timers.end())
		it->IntervalMs = intervalMs;
}

void SOUND_StartTimer(int timerIdx) {
	std::lock_guard<std::recursive_mutex> lock(TimersMutex);
	auto it = FindTimer(timerIdx);
	if (it != Timers.end()) {
		it->Running = true;
		it->NextTick = std::chrono::steady_clock::now() + std::chrono::milliseconds(it->IntervalMs);
	}
}

void SOUND_StopTimer(int timerIdx) {
	std::lock_guard<std::recursive_mutex> lock(TimersMutex);
	auto it = FindTimer(timerIdx);
	if (it != Timers.end())
		Timers.erase(it);
}

static void SOUND_UpdateTimers() {
	std::lock_guard<std::recursive_mutex> lock(TimersMutex);
	auto now = std::chrono::steady_clock::now();
	// A callback can release its own timer (AIL_release_timer_handle -> SOUND_StopTimer),
	// which erases it from the list.  Walking the list with a range-for then advanced from a
	// freed node - the crash in issue #29.  Collect the due timers first and look each one
	// up again around its callback.
	std::vector<int> due;
	for (const auto &t : Timers)
		if (t.Running && now >= t.NextTick)
			due.push_back(t.Id);
	for (int timerIdx : due) {
		auto it = FindTimer(timerIdx);
		if (it == Timers.end() || !it->Running || !it->Callback)
			continue;
		std::function<uint32_t(uint32_t)> callback = it->Callback;
		uint32_t next = callback(it->IntervalMs);
		it = FindTimer(timerIdx);
		if (it == Timers.end())
			continue;
		if (next == 0)
			it->Running = false;
		else
			it->NextTick = now + std::chrono::milliseconds(next);
	}
}

//----------------------------------------------------------------------------------------------
// Speech (CD tracks, played by Godot)
//----------------------------------------------------------------------------------------------

bool PlayCdTrackSegment(uint8_t trackIdx, int32_t startPosMs, int32_t lengthMs) {
	sound_queue_add_action("PlayCdTrackSegment", trackIdx - 1, startPosMs, lengthMs, 0);
	return true;
}

// Godot does not report the speech player; the game only asks before it starts the next one.
bool IsCdTrackPlaying() {
	return true;
}

bool EndPlayingCdTrackSegment() {
	sound_queue_add_action("EndPlayingCdTrackSegment", 0, 0, 0, 0);
	return true;
}

bool AreCdTracksAvailable() {
	return GetCdTrackCount() > 0;
}

int GetCdTrackCount() {
	return 27;
}

//----------------------------------------------------------------------------------------------
// Init / cleanup
//----------------------------------------------------------------------------------------------

bool init_sound() {
	srand(time(NULL));
	awe32Music = false;
	if (musicType == "awe32") {
		std::string error;
		awe32RomPath = Awe32Rom::Locate(awe32RomFolder, error);
		if (awe32RomPath.empty()) {
			Logger->warn("AWE32 music: {}; the ogg music plays instead", error);
			musicWarning = "AWE32 ROM missing, ogg music plays";
		} else {
			awe32Music = true;
			Awe32Mdi::SetBufferMs(awe32BufferMs);
			Logger->info("AWE32 music: wave ROM {}, buffer {} ms", awe32RomPath, awe32BufferMs);
		}
	}
	//as in remc2 (read_config.cpp) the Magic Carpet 1 music needs the AWE32 music
	if (!awe32Music)
		useMagicCarpet1Music = false;
	//Godot opens the AWE32 stream player and pulls SOUND_RenderAwe32Music; p1 = 1 when the
	//emulation plays the music, 0 when Godot plays its own ogg/MIDI music
	sound_queue_add_action("SOUND_music_mode", awe32Music ? 1 : 0, Awe32Mdi::kSampleRate, awe32BufferMs, 0);
	return true;
}

void clean_up_sound() {
	if (unitTests)
		return;
	Awe32Mdi::Shutdown();
	awe32Music = false;
	channelSamples.clear();
	sound_queue_add_action("SOUND_music_mode", 0, Awe32Mdi::kSampleRate, awe32BufferMs, 0);
}

//----------------------------------------------------------------------------------------------
// Queue for Godot
//----------------------------------------------------------------------------------------------

std::vector<SoundAction> sound_queue_get_pending_actions() {
	return sound_queue;
}

void sound_queue_clear() {
	sound_queue.clear();
	// everything queued until now is handed to Godot
	sampleStartsDelivered = sampleStartsQueued;
	SOUND_UpdateTimers();
}

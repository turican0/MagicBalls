#pragma once
#ifndef PORT_SDL_SOUND_H
#define PORT_SDL_SOUND_H

// MagicBalls: the sound requests of the engine go to Godot through sound_queue,
// see port_sdl_sound.cpp.

#include "bitmap_pos_struct.h"
#include "xmi2mid.h"
#include <time.h>
#include <string>

#include <list>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <cstdint>

#include "../engine/ail_sound.h"
#include "port_filesystem.h"

struct SoundAction {
	std::string action;
	int p1 = 0;
	int p2 = 0;
	int p3 = 0;
	int p4 = 0;
};

#pragma pack (1)
typedef struct {//lenght 32 - this is may be format of wav sound file
	int8_t filename_0[18];//first 18 chars - name//30
	uint8_t* wavData_18;//data of wav//18
	int8_t stub_22[4];//22
	int32_t wavSize_26;//24
	int16_t word_30;//28	
}
sub2type_E37A0_sound_buffer2;

typedef struct {//lenght 2072
	int8_t stub[24];
	sub2type_E37A0_sound_buffer2 wavs_10[96];
}
sub1type_E37A0_sound_buffer2;

typedef struct {//lenght 3100
	int8_t byte_0;
	int8_t byte_1;
	int8_t byte_2;
	int8_t byte_3;
	int8_t byte_4;
	int8_t byte_5;
	int8_t byte_6;
	int8_t byte_7;
	sub1type_E37A0_sound_buffer2 str_8;//2072 lenght
}
type_E37A0_sound_buffer2;

//shadow type_E37A0_sound_buffer2
typedef struct {//lenght 32 - this is may be format of wav sound file
	int8_t filename_0[18];//first 18 chars - name//30
	int32_t wavData_18;//data of wav//18
	int8_t stub_22[4];//22
	int32_t wavSize_26;//24
	int16_t word_30;//28
}
shadow_sub2type_E37A0_sound_buffer2;

typedef struct {//lenght 2072
	int8_t stub[24];
	shadow_sub2type_E37A0_sound_buffer2 wavs_10[96];
}
shadow_sub1type_E37A0_sound_buffer2;

typedef struct {//lenght 3100
	int8_t byte_0;
	int8_t byte_1;
	int8_t byte_2;
	int8_t byte_3;
	int8_t byte_4;
	int8_t byte_5;
	int8_t byte_6;
	int8_t byte_7;
	shadow_sub1type_E37A0_sound_buffer2 str_8;//2072 lenght
}
shadow_type_E37A0_sound_buffer2;
//shadow type_E37A0_sound_buffer2

typedef struct {//lenght 16*6=96
	int32_t dword_0;
	int32_t dword_4;
	int32_t sizeBytes_8;
	int32_t dword_12;
}
type_v8;



typedef struct {//lenght 32 - this is may be format of wav sound file
	uint8_t* xmiData_0;//data of wav//18
	int8_t stub_4[4];//22
	int32_t xmiSize_8;//24
	int16_t word_12;//28
	int8_t filename_14[18];//first 18 chars - name//30
}
sub2type_E3808_music_header;

typedef struct {//lenght 216
	int8_t stub[10];
	sub2type_E3808_music_header track_10[6];
	int8_t stubb[14];
}
sub1type_E3808_music_header;

typedef struct {//lenght 224
	int8_t byte_0;
	int8_t byte_1;
	int8_t byte_2;
	int8_t byte_3;
	int8_t byte_4;
	int8_t byte_5;
	int8_t byte_6;
	int8_t byte_7;
	sub1type_E3808_music_header str_8;//216 lenght
}
type_E3808_music_header;

//shadow shadow_type_E3808_music_header
typedef struct {//lenght 32 - this is may be format of wav sound file
	int8_t filename_14[18]; //first 18 chars - name//30
	int32_t xmiData_0;//data of wav//18
	int8_t stub_4[4];//22
	int32_t xmiSize_8;//24
	int16_t word_12;//28	
}
shadow_sub2type_E3808_music_header;

typedef struct {//lenght 216
	int8_t stub[10];
	int8_t stubb[14];
	shadow_sub2type_E3808_music_header track_10[6];	
}
shadow_sub1type_E3808_music_header;

typedef struct {//lenght 224
	int8_t byte_0;
	int8_t byte_1;
	int8_t byte_2;
	int8_t byte_3;
	int8_t byte_4;
	int8_t byte_5;
	int8_t byte_6;
	int8_t byte_7;
	shadow_sub1type_E3808_music_header str_8;//216 lenght
}
shadow_type_E3808_music_header;

//shadow shadow_type_E3808_music_header
#pragma pack (16)

extern bool debug_first_sound;

extern bool hqsound;
extern bool oggmusic;
extern char oggmusicFolder[512];
extern bool oggmusicalternative;
extern bool fixspeedsound;
extern bool autoShowObjectivesForForeignLanguages;
extern int maxSimultaniousSounds;
extern char speechFolder[512];

extern std::string musicType;
extern std::string awe32RomFolder;
extern bool awe32Music;//the XMI music of the game plays through the AWE32 emulation (musicType "awe32" and the ROM found)
extern bool useMagicCarpet1Music;
extern bool awe32GmIntroMusic;//the intro and the cutscenes play the General MIDI version of the XMI music instead of the AWE32 one
extern int awe32BufferMs;//how far ahead the AWE32 emulation renders, more for slow (debug) builds

void SOUND_ShowMusicWarning();
uint8_t* SOUND_DosRealPointer(uint16_t segment, uint16_t offset);
uint16_t SOUND_DosRealAlloc(uint32_t bytes);
void SOUND_DosRealFree(uint16_t segment);

bool SOUND_IsAwe32Music();
// Stereo 16-bit frames at 44100 Hz of the AWE32 emulation, pulled by Godot.
void SOUND_RenderAwe32Music(int16_t* out, int frames);

bool init_sound();
void clean_up_sound();
extern uint8_t sound_buffer[4][20000];

int32_t ac_sound_call_driver(AIL_DRIVER* drvr, int32_t fn, VDI_CALL* out);

void ac_set_real_vect(uint32_t vectnum, uint16_t real_ptr);
uint16_t ac_get_real_vect(uint32_t vectnum);

void SOUND_init_MIDI_sequence(uint8_t* datax, type_E3808_music_header* headerx, int32_t track_number);

void SOUND_start_sample(HSAMPLE S);
void SOUND_end_sample(HSAMPLE S);

void SOUND_start_sequence(int32_t sequence_num);
void WarMusicSetVolume(int32_t volume);
void SOUND_pause_sequence(int32_t sequence_num);
void SOUND_stop_sequence(int32_t sequence_num);
void SOUND_resume_sequence(int32_t sequence_num);
uint32_t SOUND_sample_status(HSAMPLE S);

void SOUND_set_sample_volume(HSAMPLE S, int32_t volume);
void SOUND_set_sequence_volume(int32_t volume, int32_t  milliseconds);
void SOUND_set_master_volume(int32_t volume);
void SOUND_set_sample_volume_panning(HSAMPLE S, int32_t panning);
void SetSamplePosition(HSAMPLE S, int16_t angle, uint8_t distance);

void SOUND_RegisterTimer(int timerIdx, uint32_t(*callback)(uint32_t));
void SOUND_SetTimerPeriod(int timerIdx, uint32_t intervalMs);
void SOUND_StartTimer(int timerIdx);
void SOUND_StopTimer(int timerIdx);

bool PlayCdTrackSegment(uint8_t trackIdx, int32_t startPosSec, int32_t lengthMs);
bool IsCdTrackPlaying();
bool EndPlayingCdTrackSegment();
bool AreCdTracksAvailable();
int GetCdTrackCount();

void sound_queue_add_action(const std::string &action, int p1, int p2, int p3, int p4);
// Godot takes the pending actions once a frame and then clears the queue (which also fires the AIL timers).
std::vector<SoundAction> sound_queue_get_pending_actions();
void sound_queue_clear();
// Which sample players of Godot play (1) or not (0), indexed by the AIL channel.
void sound_update_playing(const std::vector<int> &playing);

#endif //PORT_SDL_SOUND

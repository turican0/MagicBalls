#include "../engine/engine_support.h"
#include "port_sdl_sound.h"

//#include <adlmidi.h>
#include <iostream>

#ifdef __linux__
    #include <limits>
    #define MAX_PATH PATH_MAX
#endif

/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

bool debug_first_sound=true;

bool hqsound=false;
bool oggmusic=false;
bool oggmusicalternative = false;
char oggmusicFolder[512];

bool fixspeedsound = false;

int32_t last_sequence_num = 0;

int lastMusicVolume = -1;
int settingsMusicVolume = 127;
int num_IO_configurations = 3;
int service_rate = -1;
int master_volume = -1;

//The music that will be played
#ifdef SOUND_SDLMIXER
//Mix_Music* GAME_music[20] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
#endif//SOUND_SDLMIXER
#ifdef SOUND_OPENAL
//Mix_Music* GAME_music[20] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
ALint source_state[32];
ALuint alSource[32];      //source
ALuint alSampleSet[32];
#endif//SOUND_OPENAL
//Mix_Chunk gamechunk[32];
HSAMPLE gamechunkHSAMPLE[32];

uint8_t sound_buffer[4][20000];
/*
10
29

128-0
0-0
0-10
0-29
0-80
0-76
0-78
0-78

 channel 1
 2

*/
void test_midi_play(uint8_t*  /*data*/, uint8_t* header, int32_t track_number)
{
	uint8_t* acttrack = &header[32 + track_number * 32];
	//int testsize = *(uint32_t*)(&header[32 + (track_number + 1) * 32] + 18) - *(uint32_t*)(acttrack + 18);
	int testsize2 = *(uint32_t*)(acttrack + 26);

	//unsigned char* TranscodeXmiToMid(const unsigned char* pXmiData,	size_t iXmiLength, size_t* pMidLength);
	size_t iXmiLength = testsize2;
	size_t pMidLength;
	//uint8_t* outmidi = TranscodeXmiToMid((const uint8_t*)*(uint32_t*)(acttrack + 18), iXmiLength, &pMidLength);
	//SDL_RWops* rwmidi = SDL_RWFromMem(outmidi, pMidLength);

	//Timidity_Init();
	if (track_number > 20)
	{
		exit(0);
	}
#ifdef SOUND_SDLMIXER
	//GAME_music[track_number] = Mix_LoadMUSType_RW(rwmidi, MUS_MID, SDL_TRUE);
#endif//SOUND_SDLMIXER
	//music2 = Mix_LoadMUSType_RW(rwmidi, MIX_MUSIC_TIMIDITY, SDL_TRUE);
	playmusic2(track_number);
}

void SOUND_start_sequence(int32_t sequence_num) {
	if (unitTests)return;
	//3 - menu
	//4 - intro
#ifdef SOUND_SDLMIXER
	last_sequence_num = sequence_num;
	//volume fix
	if (lastMusicVolume == -1)
	{
		SOUND_set_sequence_volume(0x64, 0);
	}
	if (lastMusicVolume != settingsMusicVolume)
	{
		SOUND_set_sequence_volume(settingsMusicVolume, 0);
	}
	//volume fix
	/*
	if (Mix_PlayingMusic() == 0)
	{
		if (Mix_PlayMusic(GAME_music[sequence_num], -1) == -1)
			if (Mix_PausedMusic() == 1)
			{
				Mix_ResumeMusic();
			}
			else
			{
				Mix_PauseMusic();
			}
	}
	*/
#endif//SOUND_SDLMIXER
};

void SOUND_pause_sequence(int32_t  /*sequence_num*/) {
	if (unitTests)return;
#ifdef SOUND_SDLMIXER
	//Mix_PauseMusic();
#endif//SOUND_SDLMIXER
};

void SOUND_stop_sequence(int32_t  /*sequence_num*/) {
	if (unitTests)return;
#ifdef SOUND_SDLMIXER
	//Mix_HaltMusic();
#endif//SOUND_SDLMIXER
};
void SOUND_resume_sequence(int32_t  /*sequence_num*/) {
	if (unitTests)return;
#ifdef SOUND_SDLMIXER
	//Mix_ResumeMusic();
#endif//SOUND_SDLMIXER
};

void SOUND_set_sequence_volume(int32_t volume, int32_t  milliseconds) {
	return;
}

void SOUND_init_MIDI_sequence(uint8_t*  /*datax*/, type_E3808_music_header* headerx, int32_t track_number)
{
	return;
}




//Mix_Chunk mychunk;

/*bool load_sound_files()
{
	//Load the sound effects
	scratch = Mix_LoadWAV("scratch.wav");
	high = Mix_LoadWAV("high.wav");
	med = Mix_LoadWAV("medium.wav");
	low = Mix_LoadWAV("low.wav");

	//If there was a problem loading the sound effects
	if ((scratch == NULL) || (high == NULL) || (med == NULL) || (low == NULL))
	{
		return false;
	}

	//If everything loaded fine
	return true;
}*/

void clean_up_sound()
{
	if (unitTests)return;
	/*//Free the sound effects
	Mix_FreeChunk(scratch);
	Mix_FreeChunk(high);
	Mix_FreeChunk(med);
	Mix_FreeChunk(low);*/

	//Free the music
	//Mix_FreeMusic(music);
#ifdef SOUND_SDLMIXER
	for (int i = 0;i < 10;i++)
	{
		//Mix_FreeChunk(GAME_musicmp3[i]);
		//Mix_FreeMusic(GAME_music[i]);
	}

	//Quit SDL_mixer
	//Mix_CloseAudio();
#endif//SOUND_SDLMIXER
}
/*
int load_music_files() {
	GAME_musicmp3[0] = Mix_LoadMUS("music.mp3");
}*/
/*
int playsound1()
{
	//Play the scratch effect
	if (Mix_PlayChannel(-1, scratch, 0) == -1)
	{
		return 1;
	}
	return 0;
}
int playsound2()
{
	//Play the scratch effect
	if (Mix_PlayChannel(-1, high, 0) == -1)
	{
		return 1;
	}
	return 0;
}
int playsound3()
{
	//Play the scratch effect
	if (Mix_PlayChannel(-1, med, 0) == -1)
	{
		return 1;
	}
	return 0;
}
int playsound4()
{
	//Play the scratch effect
	if (Mix_PlayChannel(-1, low, 0) == -1)
	{
		return 1;
	}
	return 0;
}
*/
/*
void playmusic1()
{
	if (Mix_PlayingMusic() == 0)
	{
		//Play the music
		if (Mix_PlayMusic(music, -1) == -1)
			if (Mix_PausedMusic() == 1)
			{
				//Resume the music
				Mix_ResumeMusic();
			}
		//If the music is playing
			else
			{
				//Pause the music
				Mix_PauseMusic();
			}
	}
}
*/
/*
void stopmusic1()
{
	Mix_HaltMusic();
}
*/
void playmusic2(int32_t track_number)
{
	return;
}

struct {
	int a;
} common_IO_configurations;

struct {
	int a;
} environment_string;

int32_t ac_sound_call_driver(AIL_DRIVER* drvr, int32_t fn, VDI_CALL* out) {
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
	case 0x304: {//AIL_API_install_DIG_driver_file/AIL_API_install_MDI_driver_file
		break;
	}
	case 0x305: {//AIL_API_install_DIG_driver_file/AIL_API_install_MDI_driver_file
		break;
	}
	case 0x306: {//AIL_API_uninstall_driver/AIL_API_uninstall_MDI_driver_file
		break;
	}
	case 0x401: {
		/*		mychunk.abuf=(uint8_t*)last_sample->start_2_3[0];
				mychunk.alen = last_sample->len_4_5[0];
				mychunk.volume = last_sample->volume_16;
				//mychunk.allocated = 0;

				#ifdef USE_SDL2
						Mix_PlayChannel(-1, &mychunk, 0);
				#else
						Mix_PlayChannel(-1, &mychunk, 0);
				#endif*/
		break;
	}
	case 0x501: {//AIL_API_install_MDI_INI
		break;
	}
	case 0x502: {//AIL_API_install_MDI_INI
		break;
	}
	default:
	{
		break;
	}
	}
	return 1;
};

void SOUND_set_master_volume(int32_t volume) {
	sound_queue_add_action("SOUND_set_master_volume", 0, 0);
}

void SOUND_set_sample_volume(HSAMPLE S, int32_t volume) {
	sound_queue_add_action("SOUND_set_sample_volume", 0, 0);
}

void SOUND_start_sample(HSAMPLE S) {
	sound_queue_add_action("SOUND_end_sample", 0, 0);
};

uint32_t SOUND_sample_status(HSAMPLE S) {
	sound_queue_add_action("SOUND_sample_status", 0, 0);
	return 0;
}

void SOUND_end_sample(HSAMPLE  /*S*/) {
	sound_queue_add_action("SOUND_end_sample", 0, 0);
};

void SOUND_finalize(int channel) {
	HSAMPLE S = gamechunkHSAMPLE[channel];
	if (S)
	{
		if (S->status_1 != 1)
		{
			if (S->status_1 != 2)
			{
				S->status_1 = 2;
			}
		}
		S->vol_scale_18[0][0] = 0;
		S->flags_14 = 0;
		S->vol_scale_18[0][2] = 0;
		S->vol_scale_18[0][3] = 0;
	}
	sound_queue_add_action("SOUND_finalize", 0, 0);
}

int run();

bool init_sound()
{
	return true;
}

AIL_DRIVER* ac_AIL_API_install_driver(int  /*a1*/, uint8_t*  /*a2*/, int  /*a3*/)/*driver_image,n_bytes*///27f720
{


	//printf("drvr:%08X, fn:%08X, in:%08X, out:%08X\n", drvr, fn, in, out);
	return 0;
}

uint16_t actvect[4096];

void ac_set_real_vect(uint32_t vectnum, uint16_t real_ptr)
{
	actvect[vectnum] = real_ptr;
	//66
};

uint16_t ac_get_real_vect(uint32_t vectnum)
{
	return actvect[vectnum];
};

void test_music()
{
}

void my_audio_callback(void *midi_player, uint8_t *stream, int len);

/* variable declarations */
static uint32_t is_playing = 0; /* remaining length of the sample we have to play */
static short buffer[4096]; /* Audio buffer */

int run()
{
	return 0;
}

/*
 audio callback function
 here you have to copy the data of your audio buffer into the
 requesting audio buffer (stream)
 you should only copy as much as the requested length (len)
*/
void my_audio_callback(void *midi_player, uint8_t *stream, int len)
{
}

#define TEST_ERROR(_msg)		\
	error = alGetError();		\
	if (error != AL_NO_ERROR) {	\
		fprintf(stderr, _msg "\n");	\
		return;		\
	}
//------------------

void SOUND_UPDATE() {
#ifdef SOUND_OPENAL
	for (int i = 0; i < 32; i++)
	{
		if (source_state[i] == AL_PLAYING)
		{
			alGetSourcei(alSource[i], AL_SOURCE_STATE, &source_state[i]);
			//TEST_ERROR("source state get");
		}
		else
		{
			alDeleteSources(1, &alSource[i]);
			alDeleteBuffers(1, &alSampleSet[i]);
		}
	}
	/*while (source_state == AL_PLAYING) {
		alGetSourcei(alSource, AL_SOURCE_STATE, &source_state);
		TEST_ERROR("source state get");
	}*/
#endif//SOUND_OPENAL
};


#ifdef SOUND_OPENAL
ALCcontext* context;
ALCdevice* device;
const ALCchar* defaultDeviceName;
ALCenum error;

static void list_audio_devices(const ALCchar* devices)
{
	const ALCchar* device = devices, * next = devices + 1;
	size_t len = 0;

	fprintf(stdout, "Devices list:\n");
	fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0') {
		fprintf(stdout, "%s\n", device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	fprintf(stdout, "----------\n");
}
void ALSOUND_init()
{
	ALboolean enumeration;
	const ALCchar* devices;
	//const ALCchar* defaultDeviceName = argv[1];
	int ret;
#ifdef LIBAUDIO
	WaveInfo* wave;
#endif
	char* bufferData;
	ALCdevice* device;
	ALvoid* data;
	ALCcontext* context;
	ALsizei size, freq;
	ALenum format;
	//ALuint buffer, source;
	ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	ALboolean loop = AL_FALSE;

	//fprintf(stdout, "Using " BACKEND " as audio backend\n");

	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE)
		fprintf(stderr, "enumeration extension not available\n");

	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	//if (!defaultDeviceName)
	defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	device = alcOpenDevice(defaultDeviceName);
	if (!device) {
		fprintf(stderr, "unable to open default device\n");
		return;
	}

	fprintf(stdout, "Device: %s\n", alcGetString(device, ALC_DEVICE_SPECIFIER));

	alGetError();

	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context)) {
		fprintf(stderr, "failed to make default context\n");
		return;
	}
	TEST_ERROR("make default context");

	/* set orientation */
	alListener3f(AL_POSITION, 0, 0, 1.0f);
	TEST_ERROR("listener position");
	alListener3f(AL_VELOCITY, 0, 0, 0);
	TEST_ERROR("listener velocity");
	alListenerfv(AL_ORIENTATION, listenerOri);
	TEST_ERROR("listener orientation");
	/*
	for (int i = 0; i < 32; i++)
	{
		alGenSources((ALuint)1, &alSource[i]);
		TEST_ERROR("source generation");

		alSourcef(alSource[i], AL_PITCH, 1);
		TEST_ERROR("source pitch");
		alSourcef(alSource[i], AL_GAIN, 1);
		TEST_ERROR("source gain");
		alSource3f(alSource[i], AL_POSITION, 0, 0, 0);
		TEST_ERROR("source position");
		alSource3f(alSource[i], AL_VELOCITY, 0, 0, 0);
		TEST_ERROR("source velocity");
		alSourcei(alSource[i], AL_LOOPING, AL_FALSE);
		TEST_ERROR("source looping");

		alGenBuffers(1, &alSampleSet[i]);
		TEST_ERROR("buffer generation");
	}*/
}

ALenum alFormatBuffer = AL_FORMAT_MONO16/*AL_FORMAT_MONO16*/;    //buffer format
ALsizei alFreqBuffer = 44100;       //frequency
//ALint source_state;
/*void ALSOUND_load_wav(char* alBuffer,long alBufferLen)
{
	//Now for the second part, loading a wav file.You have to open a file, fill buffers with dataand then attach it to a source.

	//char* alBuffer;             //data for the buffer
	
	//long       alBufferLen;        //bit depth
	//ALboolean    alLoop;         //loop


	//load the wave file
	//alutLoadWAVFile("my_music.wav", alFormatBuffer, (void**) alBuffer, (unsigned int*) alBufferLen, alFreqBuffer);//, alLoop);

	//create a source
	alGenSources(1, &alSource);

	//create  buffer
	alGenBuffers(1, &alSampleSet);

	//put the data into our sampleset buffer
	alBufferData(alSampleSet, alFormatBuffer, alBuffer, alBufferLen, alFreqBuffer);

	//assign the buffer to this source
	alSourcei(alSource, AL_BUFFER, alSampleSet);

	//release the data
	//alutUnloadWAV(alFormatBuffer, alBuffer, alBufferLen, alFreqBuffer);
}*/
void ALSOUND_play(int which, Mix_Chunk* mixchunk, int loops)
{
	/*
	//Once the sound is loaded we can play it.To do this we use alSourcePlay.

	alSourcei(alSource, AL_LOOPING, AL_TRUE);

	//play
	alSourcePlay(alSource);

	//alSourcePlay(source);

	//to stop
	//alSourceStop(alSource);
	*/
	/*
	alSourcePlay(): Play, replay, or resume a source.
	alSourceStop(): Stop one or more sources.
	alSourceRewind(): Rewind a source (set the playback position to the beginning).
	alSourcePause(): Pause a source.
	*/
	//alutLoadWAVFile("test.wav", &format, &data, &size, &freq, &loop);
	//TEST_ERROR("loading wav file");
	
	//----
	//delete
	//alDeleteSources(1, &alSource);
	//delete our buffer
	//alDeleteBuffers(1, &alSampleSet);
    //----
	alGenSources((ALuint)1, &alSource[which]);
	TEST_ERROR("source generation");

	alSourcef(alSource[which], AL_PITCH, 1);
	TEST_ERROR("source pitch");
	alSourcef(alSource[which], AL_GAIN, 1);
	TEST_ERROR("source gain");
	alSource3f(alSource[which], AL_POSITION, 0, 0, 0);
	TEST_ERROR("source position");
	alSource3f(alSource[which], AL_VELOCITY, 0, 0, 0);
	TEST_ERROR("source velocity");
	alSourcei(alSource[which], AL_LOOPING, AL_FALSE);
	TEST_ERROR("source looping");

	alGenBuffers(1, &alSampleSet[which]);
	TEST_ERROR("buffer generation");
	//------


	alBufferData(alSampleSet[which], alFormatBuffer, mixchunk->abuf, mixchunk->alen, alFreqBuffer);
	TEST_ERROR("buffer copy");

	alSourcei(alSource[which], AL_BUFFER, alSampleSet[which]);
	TEST_ERROR("buffer binding");

	alSourcePlay(alSource[which]);
	TEST_ERROR("source playing");

	alGetSourcei(alSource[which], AL_SOURCE_STATE, &source_state[which]);
	TEST_ERROR("source state get");
	/*while (source_state == AL_PLAYING) {
		alGetSourcei(alSource, AL_SOURCE_STATE, &source_state);
		TEST_ERROR("source state get");
	}

	alDeleteSources(1, &alSource);
	alDeleteBuffers(1, &alSampleSet);*/
}

void ALSOUND_delete()
{
	//Once you�ve finished don�t forget to clean memoryand release OpenAL contextand device

	//alDeleteSources(1, &alSource);

	//delete our buffer
	//alDeleteBuffers(1, &alSampleSet);

	context = alcGetCurrentContext();

	//Get device for active context
	device = alcGetContextsDevice(context);

	//Disable context
	alcMakeContextCurrent(NULL);

	//Release context(s)
	alcDestroyContext(context);

	//Close device
	alcCloseDevice(device);
}


/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdbool.h>

#include <AL/al.h>
#include <AL/alc.h>
*/
/*
#ifdef LIBAUDIO
#include <audio/wave.h>
#define BACKEND	"libaudio"
#else
#include <AL/alut.h>
#define BACKEND "alut"
#endif
*/



	
static inline ALenum to_al_format(short channels, short samples)
{
	bool stereo = (channels > 1);

	switch (samples) {
	case 16:
		if (stereo)
			return AL_FORMAT_STEREO16;
		else
			return AL_FORMAT_MONO16;
	case 8:
		if (stereo)
			return AL_FORMAT_STEREO8;
		else
			return AL_FORMAT_MONO8;
	default:
		return -1;
	}
}
#endif//SOUND_OPENAL
/*
int main(int argc, char** argv)
{
	ALboolean enumeration;
	const ALCchar* devices;
	const ALCchar* defaultDeviceName = argv[1];
	int ret;
#ifdef LIBAUDIO
	WaveInfo* wave;
#endif
	char* bufferData;
	ALCdevice* device;
	ALvoid* data;
	ALCcontext* context;
	ALsizei size, freq;
	ALenum format;
	ALuint buffer, source;
	ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	ALboolean loop = AL_FALSE;
	ALCenum error;
	ALint source_state;

	//fprintf(stdout, "Using " BACKEND " as audio backend\n");

	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE)
		fprintf(stderr, "enumeration extension not available\n");

	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	if (!defaultDeviceName)
		defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	device = alcOpenDevice(defaultDeviceName);
	if (!device) {
		fprintf(stderr, "unable to open default device\n");
		return -1;
	}

	fprintf(stdout, "Device: %s\n", alcGetString(device, ALC_DEVICE_SPECIFIER));

	alGetError();

	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context)) {
		fprintf(stderr, "failed to make default context\n");
		return -1;
	}
	TEST_ERROR("make default context");

	alListener3f(AL_POSITION, 0, 0, 1.0f);
	TEST_ERROR("listener position");
	alListener3f(AL_VELOCITY, 0, 0, 0);
	TEST_ERROR("listener velocity");
	alListenerfv(AL_ORIENTATION, listenerOri);
	TEST_ERROR("listener orientation");

	alGenSources((ALuint)1, &source);
	TEST_ERROR("source generation");

	alSourcef(source, AL_PITCH, 1);
	TEST_ERROR("source pitch");
	alSourcef(source, AL_GAIN, 1);
	TEST_ERROR("source gain");
	alSource3f(source, AL_POSITION, 0, 0, 0);
	TEST_ERROR("source position");
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	TEST_ERROR("source velocity");
	alSourcei(source, AL_LOOPING, AL_FALSE);
	TEST_ERROR("source looping");

	alGenBuffers(1, &buffer);
	TEST_ERROR("buffer generation");

#ifdef LIBAUDIO
	wave = WaveOpenFileForReading("test.wav");
	if (!wave) {
		fprintf(stderr, "failed to read wave file\n");
		return -1;
	}

	ret = WaveSeekFile(0, wave);
	if (ret) {
		fprintf(stderr, "failed to seek wave file\n");
		return -1;
	}

	bufferData = malloc(wave->dataSize);
	if (!bufferData) {
		perror("malloc");
		return -1;
	}

	ret = WaveReadFile(bufferData, wave->dataSize, wave);
	if (ret != wave->dataSize) {
		fprintf(stderr, "short read: %d, want: %d\n", ret, wave->dataSize);
		return -1;
	}

	alBufferData(buffer, to_al_format(wave->channels, wave->bitsPerSample),
		bufferData, wave->dataSize, wave->sampleRate);
	TEST_ERROR("failed to load buffer data");
#else
	//alutLoadWAVFile("test.wav", &format, &data, &size, &freq, &loop);
	TEST_ERROR("loading wav file");

	alBufferData(buffer, format, data, size, freq);
	TEST_ERROR("buffer copy");
#endif

	alSourcei(source, AL_BUFFER, buffer);
	TEST_ERROR("buffer binding");

	alSourcePlay(source);
	TEST_ERROR("source playing");

	alGetSourcei(source, AL_SOURCE_STATE, &source_state);
	TEST_ERROR("source state get");
	while (source_state == AL_PLAYING) {
		alGetSourcei(source, AL_SOURCE_STATE, &source_state);
		TEST_ERROR("source state get");
	}

	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	return 0;
}
*/

/*
Array action_queue;

void add_sound_action(String p_action, int p_p1, int p_p2) {
	Dictionary d;
	d.set(StringName("action"), Variant(p_action));
	d.set(StringName("p1"), Variant(p_p1));
	d.set(StringName("p2"), Variant(p_p2));
	action_queue.append(d);
}

void clean_actions() {
	action_queue.clear();
}

Array get_pending_actions() {
	return action_queue;
}
*/

std::vector<SoundAction> sound_queue;

void sound_queue_add_action(const std::string &action, int p1, int p2) {
	SoundAction sa;
	sa.action = action;
	sa.p1 = p1;
	sa.p2 = p2;
	sound_queue.push_back(sa);
}

void sound_queue_add_actionSA(const SoundAction &sa) {
	sound_queue.push_back(sa);
}

std::vector<SoundAction> sound_queue_get_pending_actions() {
	return sound_queue;
}

void sound_queue_clear() {
	sound_queue.clear();
}

size_t sound_queue_size() {
	return sound_queue.size();
}

bool sound_queue_empty() {
	return sound_queue.empty();
}

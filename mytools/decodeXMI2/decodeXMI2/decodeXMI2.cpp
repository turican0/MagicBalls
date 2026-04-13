// decodeXMI2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <cstring>   // pro memcpy
#include <cstdio>
#include <vector>

__int16 m_iNumberOfTracks = 0;
int countOfMusicTracks_E380C = 0;
uint8_t* musicData_E3810 = nullptr;
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
/*
typedef struct {//lenght 80656
int8_t byte_0;
int8_t byte_1;
int8_t byte_2;
int8_t byte_3;
int8_t byte_4;
int8_t byte_5;
int8_t byte_6;
int8_t byte_7;
int8_t data_8[];
}
type_E3810_music_data;*/
//shadow shadow_type_E3808_music_header
typedef struct {//lenght 32 - this is may be format of wav sound file
    int32_t xmiData_0;//data of wav//18
    int8_t stub_4[4];//22
    int32_t xmiSize_8;//24
    int16_t word_12;//28
    int8_t filename_14[18];//first 18 chars - name//30
}
shadow_sub2type_E3808_music_header;
typedef struct {//lenght 216
    int8_t stub[10];
    shadow_sub2type_E3808_music_header track_10[6];
    int8_t stubb[14];
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
type_E3808_music_header* musicHeader_E3808 = nullptr;

// decodeXMI2.cpp
//#include <iostream>
//#include <cstring>
//#include <cstdio>
/*
__int16 m_iNumberOfTracks = 0;
int countOfMusicTracks_E380C = 0;
uint8_t* musicData_E3810 = nullptr;

#pragma pack (1)
typedef struct { int8_t filename_0[18]; uint8_t* wavData_18; int8_t stub_22[4]; int32_t wavSize_26; int16_t word_30; } sub2type_E37A0_sound_buffer2;
typedef struct { int8_t stub[24]; sub2type_E37A0_sound_buffer2 wavs_10[96]; } sub1type_E37A0_sound_buffer2;
typedef struct { int8_t byte_0; int8_t byte_1; int8_t byte_2; int8_t byte_3; int8_t byte_4; int8_t byte_5; int8_t byte_6; int8_t byte_7; sub1type_E37A0_sound_buffer2 str_8; } type_E37A0_sound_buffer2;
typedef struct { uint8_t* xmiData_0; int8_t stub_4[4]; int32_t xmiSize_8; int16_t word_12; int8_t filename_14[18]; } sub2type_E3808_music_header;
typedef struct { int8_t stub[10]; sub2type_E3808_music_header track_10[6]; int8_t stubb[14]; } sub1type_E3808_music_header;
typedef struct { int8_t byte_0; int8_t byte_1; int8_t byte_2; int8_t byte_3; int8_t byte_4; int8_t byte_5; int8_t byte_6; int8_t byte_7; sub1type_E3808_music_header str_8; } type_E3808_music_header;
typedef struct { int32_t dword_0; int32_t dword_4; int32_t sizeBytes_8; int32_t dword_12; } type_v8;
#pragma pack (16)*/

//type_E3808_music_header* musicHeader_E3808 = nullptr;

void GetMusicSequenceCount() {
    for (m_iNumberOfTracks = 0; m_iNumberOfTracks < countOfMusicTracks_E380C; m_iNumberOfTracks++) {
        uint8_t* zero_pointer = 0;
        musicHeader_E3808->str_8.track_10[m_iNumberOfTracks].xmiData_0 = &musicData_E3810[musicHeader_E3808->str_8.track_10[m_iNumberOfTracks].xmiData_0 - zero_pointer];
    }
}

uint8_t* Zero_pointer = NULL;
int filelenght;

bool LoadMusicTrack(FILE* filehandle,uint8_t* fullMusicData, type_v8 headerx[4], uint8_t drivernumber) {
    int rncsize;

    ftell(filehandle);
    fread((uint8_t*)headerx, 1, 64, filehandle);
    //DataFileIO::Read(filehandle, (uint8_t*)headerx, 64);
    if (headerx[drivernumber].dword_4 == -1)
        return false;
    if (musicData_E3810)
    {
        //FreeMem_83E80(musicData_E3810);
        //musicAble_E37FC = false;
    }
    if (musicHeader_E3808)
    {
        //FreeMem_83E80((uint8_t*)musicHeader_E3808);
        //musicAble_E37FC = false;
    }
    musicData_E3810 = (uint8_t*)malloc(headerx[drivernumber].dword_12);
    if (!musicData_E3810)
        return false;
    musicHeader_E3808 = (type_E3808_music_header*)malloc(sizeof(type_E3808_music_header));
    if (!musicHeader_E3808)
    {
        //FreeMem_83E80(musicData_E3810);
        return false;
    }

    //64xbit fix
    shadow_type_E3808_music_header* shadow_str_E3808_music_header = (shadow_type_E3808_music_header*)malloc(sizeof(shadow_type_E3808_music_header));
    if (!shadow_str_E3808_music_header)
    {
        //FreeMem_83E80((uint8_t*)shadow_str_E3808_music_header);
        return false;
    }
    //64xbit fix

    countOfMusicTracks_E380C = (headerx[drivernumber].sizeBytes_8) / sizeof(sub2type_E37A0_sound_buffer2);
    fseek(filehandle, headerx[drivernumber].dword_4, 0);
    fread(musicData_E3810, 1, 8, filehandle);
    //DataFileIO::Read(filehandle, musicData_E3810, 8);
    /*if (musicData_E3810[0] == 'R' && musicData_E3810[1] == 'N' && musicData_E3810[2] == 'C')//RNC
    {
        rncsize = musicData_E3810[4];
        rncsize <<= 8;
        rncsize += musicData_E3810[5];
        rncsize <<= 8;
        rncsize += musicData_E3810[6];
        rncsize <<= 8;
        rncsize += musicData_E3810[7];
        fread((uint8_t*)&musicData_E3810[8], 1, rncsize - 8, filehandle);
        //DataFileIO::Read(filehandle, (uint8_t*)&musicData_E3810[8], rncsize - 8);
        DataFileRNC::Decompress(musicData_E3810, musicData_E3810);
    }
    else*/
    {
        fread((uint8_t*)&musicData_E3810[8], 1, headerx[drivernumber].dword_12 - 8, filehandle);
        //DataFileIO::Read(filehandle, (uint8_t*)&musicData_E3810[8], headerx[drivernumber].dword_12 - 8);
    }
    fseek(filehandle, headerx[drivernumber].dword_0, 0);

    //64x fix shadow_str_E3808_music_header
    fread((uint8_t*)shadow_str_E3808_music_header, 1, 8, filehandle);
    //DataFileIO::Read(filehandle, (uint8_t*)shadow_str_E3808_music_header, 8);
    /*if (shadow_str_E3808_music_header->byte_0 == 'R' && shadow_str_E3808_music_header->byte_1 == 'N' && shadow_str_E3808_music_header->byte_2 == 'C')//RNC
    {
        rncsize = shadow_str_E3808_music_header->byte_4;
        rncsize <<= 8;
        rncsize += shadow_str_E3808_music_header->byte_5;
        rncsize <<= 8;
        rncsize += shadow_str_E3808_music_header->byte_6;
        rncsize <<= 8;
        rncsize += shadow_str_E3808_music_header->byte_7;
        DataFileIO::Read(filehandle, (uint8_t*)(&shadow_str_E3808_music_header->str_8), rncsize - 8);
        DataFileRNC::Decompress((uint8_t*)shadow_str_E3808_music_header, (uint8_t*)shadow_str_E3808_music_header);
    }
    else*/
    {
        fread((uint8_t*)(&shadow_str_E3808_music_header->str_8), 1, headerx[drivernumber].sizeBytes_8 - 8, filehandle);
        //DataFileIO::Read(filehandle, (uint8_t*)(&shadow_str_E3808_music_header->str_8), headerx[drivernumber].sizeBytes_8 - 8);
    }
    musicHeader_E3808->byte_0 = shadow_str_E3808_music_header->byte_0;
    musicHeader_E3808->byte_1 = shadow_str_E3808_music_header->byte_1;
    musicHeader_E3808->byte_2 = shadow_str_E3808_music_header->byte_2;
    musicHeader_E3808->byte_3 = shadow_str_E3808_music_header->byte_3;
    musicHeader_E3808->byte_4 = shadow_str_E3808_music_header->byte_4;
    musicHeader_E3808->byte_5 = shadow_str_E3808_music_header->byte_5;
    musicHeader_E3808->byte_6 = shadow_str_E3808_music_header->byte_6;
    musicHeader_E3808->byte_7 = shadow_str_E3808_music_header->byte_7;
    for (int i = 0; i < 10; i++)
        musicHeader_E3808->str_8.stub[i] = shadow_str_E3808_music_header->str_8.stub[i];
    for (int i = 0; i < 6; i++)
    {
        musicHeader_E3808->str_8.track_10[i].xmiData_0 = shadow_str_E3808_music_header->str_8.track_10[i].xmiData_0 + Zero_pointer;
        for (int j = 0; j < 4; j++)
            musicHeader_E3808->str_8.track_10[i].stub_4[j] = shadow_str_E3808_music_header->str_8.track_10[i].stub_4[j];
        musicHeader_E3808->str_8.track_10[i].xmiSize_8 = shadow_str_E3808_music_header->str_8.track_10[i].xmiSize_8;
        musicHeader_E3808->str_8.track_10[i].word_12 = shadow_str_E3808_music_header->str_8.track_10[i].word_12;
        for (int j = 0; j < 18; j++)
            musicHeader_E3808->str_8.track_10[i].filename_14[j] = shadow_str_E3808_music_header->str_8.track_10[i].filename_14[j];
    }
    for (int i = 0; i < 14; i++)
        musicHeader_E3808->str_8.stubb[i] = shadow_str_E3808_music_header->str_8.stubb[i];

    GetMusicSequenceCount();

    struct trackType { uint8_t* begin; uint8_t* end; };
    trackType track[6];
    for (int i = 1; i <= m_iNumberOfTracks; i++) {
        track[i - 1].begin = musicHeader_E3808->str_8.track_10[i - 1].xmiData_0;
        track[i - 1].end = track[i - 1].begin + (uint16_t)musicHeader_E3808->str_8.track_10[i - 1].xmiSize_8;
		FILE* outFile = nullptr;
        fopen_s(&outFile,(char*)musicHeader_E3808->str_8.track_10[i - 1].filename_14,"wb");
		fwrite(track[i - 1].begin, 1, track[i - 1].end - track[i - 1].begin, outFile);
        fclose(outFile);
    }

    const char* pattern = "XDIRINFO";
    size_t patternLen = strlen(pattern);
    long positions[50];
    //if (filelenght < patternLen) return;
    int index = 0;
    for (size_t i = 0; i <= filelenght - patternLen; ++i) {
        if (memcmp(&fullMusicData[i], pattern, patternLen) == 0) {
            positions[index]=i;
            index++;
        }
    }

    for (int i = 0; i < index; i++) {
        long pos0 = positions[i] - 8;
        uint8_t* begin = &fullMusicData[pos0];
        uint8_t* end;
        if (i < index - 1)
        {
            long pos1 = positions[i+1] - 8;
            end = &fullMusicData[pos1];
        }
        else
            end = &fullMusicData[filelenght];
        FILE* outFile = nullptr;
        char path[512];
        sprintf_s(path, "xmifile%2d.XMI", i);
        fopen_s(&outFile, path, "wb");
        fwrite(begin, 1, end - begin, outFile);
        fclose(outFile);
    }


    // ===================================================================
    // <<< TADY NASTAVUJEŠ KTERÉ MIDI KANÁLY CHCEŠ ZTIŠIT >>>
    // ===================================================================
    bool muteChannels[16] = { false };   // default vše nahlas

    // <<< ZMÌÒ SI TADY >>>
    muteChannels[7] = true;   // kanál 7
    muteChannels[8] = true;   // kanál 8
    // pøíklad: muteChannels[15] = true; // kanál 16

    printf("=== Processing song - Mute settings for MIDI channels: ");
    for (int c = 0; c < 16; c++)
        printf("Ch%d:%s ", c + 1, muteChannels[c] ? "MUTE" : "LOUD");
    printf("===\n");

    for (int i = 0; i < m_iNumberOfTracks; i++) {
        printf("--- Parsing XMI track %d ---\n", i);
        uint8_t* currentPos = track[i].begin;
        uint8_t* endPos = track[i].end;

        while (currentPos < endPos) {
            if (*currentPos == 0xFF && (currentPos + 1 < endPos) && *(currentPos + 1) < 0x80) { currentPos += 3; continue; }
            while (currentPos < endPos && *currentPos >= 0x80 && *currentPos != 0xFF) currentPos++;
            if (currentPos >= endPos) break;

            uint8_t status = *currentPos;
            uint8_t type = status & 0xF0;
            uint8_t channel = status & 0x0F;   // 0-15 = MIDI channel 1-16

            if (type == 0xB0) {   // CONTROL CHANGE
                uint8_t cc = *(currentPos + 1);
                uint8_t val = *(currentPos + 2);
                if (cc == 0x07 || cc == 0x0B) {   // Main Volume nebo Expression
                    uint8_t newVal = muteChannels[channel] ? 0x00 : 0x7F;
                    if (val != newVal) {
                        *(currentPos + 2) = newVal;
                        printf("[OVERWRITE] Channel %d CC%02X: %02X -> %02X (%s)\n",
                            channel + 1, cc, val, newVal, muteChannels[channel] ? "MUTED" : "LOUD");
                    }
                }
                currentPos += 3; continue;
            }

            if (type == 0x90) {
                currentPos += 3;
                while (currentPos < endPos && (*currentPos & 0x80)) currentPos++;
                currentPos++;
                continue;
            }

            if (status == 0xFF) {
                uint8_t metaType = *(currentPos + 1);
                if (metaType == 0x50) {
                    uint8_t globalVol = *(currentPos + 3);
                    uint8_t newVal = muteChannels[channel] ? 0x00 : 0x7F;   // i globální volume pøes kanál
                    if (globalVol != newVal) *(currentPos + 3) = newVal;
                    currentPos += 4;
                }
                else if (metaType == 0x2F) currentPos += 3;
                else { uint8_t len = *(currentPos + 2); currentPos += 3 + len; }
                continue;
            }
            currentPos++;
        }
    }

    uint32_t musicDataOffset = headerx[drivernumber].dword_4;
    uint32_t musicDataSize = headerx[drivernumber].dword_12;

    memcpy(fullMusicData + musicDataOffset, musicData_E3810, musicDataSize);
    printf("=== Volumes for selected MIDI channels overwritten ===\n");
    return true;
}

int main() {
    int channel = 0;                    // <<< kterou skladbu chceš upravit (0 = první, 1 = druhá...)
    int finaldrivernumber = 2;

    std::string cdDataPath = "c:/COMPUTES/140/prenos/remc2-dev2/remc2/x64/Debug/CD_Files";
    char musicPath[512];
    sprintf_s(musicPath, "%s/SOUND/MUSIC.DAT", cdDataPath.c_str());

    FILE* filehandle = nullptr;
    if (fopen_s(&filehandle, musicPath, "rb") != 0 || !filehandle) return false;

    fseek(filehandle, 0, 2);
    filelenght = ftell(filehandle);
    fseek(filehandle, 0, 0);

    uint8_t* fullMusicData = (uint8_t*)malloc(filelenght);
    fread(fullMusicData, 1, filelenght, filehandle);


    int datapos_mem;
    int16_t driverarray_mem[4];
    memcpy(&datapos_mem, fullMusicData + filelenght - 4, 4);
    memcpy(driverarray_mem, fullMusicData + datapos_mem, 8);

    type_v8 headerx[4];
    bool loaded = false;
    int channellplus = channel + 1;
    if (channellplus <= driverarray_mem[finaldrivernumber]) {
        uint32_t dirEntryPos = (uint32_t)datapos_mem + 8 + (uint32_t)(channellplus - 1) * 64;
        memcpy(headerx, fullMusicData + dirEntryPos, 64);
        if (LoadMusicTrack(filehandle, fullMusicData, headerx, finaldrivernumber))
            loaded = true;
    }
    fclose(filehandle);

    if (!loaded) { free(fullMusicData); std::cin.get(); return true; }

    char musicPathOut[512];
    sprintf_s(musicPathOut, "%s/SOUND/MUSIC2.dat", cdDataPath.c_str());
    FILE* outhandle = nullptr;
    if (fopen_s(&outhandle, musicPathOut, "wb") == 0 && outhandle) {
        fwrite(fullMusicData, 1, filelenght, outhandle);
        fclose(outhandle);
        printf("\n=== DONE! MUSIC2.dat created - volumes pøepsány podle kanálù ===\n");
    }

    free(fullMusicData);
    std::cin.get();
    return false;
}
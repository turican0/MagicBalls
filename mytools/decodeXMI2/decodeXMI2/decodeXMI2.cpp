// decodeXMI2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

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

void GetMusicSequenceCount()//26fc90 // set index
{
	//if (musicHeader_E3808 && musicData_E3810)
	{
		for (m_iNumberOfTracks = 0; m_iNumberOfTracks < countOfMusicTracks_E380C; m_iNumberOfTracks++)
		{
			uint8_t* zero_pointer = 0;
			musicHeader_E3808->str_8.track_10[m_iNumberOfTracks].xmiData_0 = &musicData_E3810[musicHeader_E3808->str_8.track_10[m_iNumberOfTracks].xmiData_0 - zero_pointer];
		}
	}
}

uint8_t* Zero_pointer = NULL;

int filelenght;

void printTag(uint8_t* pos) {
	printf("Tag found: %c%c%c%c\n", pos[0], pos[1], pos[2], pos[3]);
}

bool LoadMusicTrack(FILE* filehandle, uint8_t drivernumber)//26fd00
{
	type_v8 headerx[4];
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
	//FreeMem_83E80((uint8_t*)shadow_str_E3808_music_header);
	//64x fix
	GetMusicSequenceCount();
	
	typedef struct {
		uint8_t* begin;
		uint8_t* end;
	} trackType;
			
	trackType track[6];

	for (int i = 1; i <= m_iNumberOfTracks; i++)
	{
		track[i-1].begin = musicHeader_E3808->str_8.track_10[i - 1].xmiData_0;
		track[i-1].end = musicHeader_E3808->str_8.track_10[i - 1].xmiData_0 + musicHeader_E3808->str_8.track_10[i - 1].xmiSize_8;
	}

for (int i = 0; i < m_iNumberOfTracks; i++)
{
    printf("--- Parse of track %d ---\n", i);

    uint8_t* currentPos = track[i].begin;
    uint8_t* endPos = track[i].end;
    uint32_t ticks = 0;

    while (currentPos < endPos)
    {
        // --- 1. DEKÓDOVÁNÍ XMI DELTA TIMU ---
        // XMI èasto používá 0xFF pro prodloužení èasu nebo specifické IFF tagy.
        // V datech vidíme "?!" (FF 21) nebo "?X" (FF 58).
        if (*currentPos == 0xFF && (currentPos + 1 < endPos) && *(currentPos + 1) < 0x80) {
            // Jednoduché pøeskoèení Meta/Time eventù, které nejsou MIDI statusy
            currentPos += 3; 
            continue;
        }

        // Pokud narazíme na hodnoty z tvého dumpu jako '?', 
        // musíme je zpracovat jako Extended Delta Time.
        while (currentPos < endPos && *currentPos >= 0x80 && *currentPos != 0xFF) {
            currentPos++; // Pøeskakujeme VLQ/XMI èasové byty
        }

        if (currentPos >= endPos) break;

        uint8_t status = *currentPos;
        uint8_t type = status & 0xF0;
        uint8_t channel = status & 0x0F;

        // ---- 2. CONTROL CHANGE (0xB0) ----
        if (type == 0xB0)
        {
            uint8_t cc = *(currentPos + 1);
            uint8_t val = *(currentPos + 2);

            if (cc == 0x07) {
                printf("[VOLUME] Ch %d: Main Volume = %02X (%d)\n", channel + 1, val, val);
            }
            else if (cc == 0x0B) {
                printf("[VOLUME] Ch %d: Expression = %02X\n", channel + 1, val);
            }
            else if (cc == 0x0A) { // Pøidáno: Paning (mùže simulovat ticho v jednom kanálu)
                printf("[PAN] Ch %d: Pan L/R = %02X\n", channel + 1, val);
            }
            else if (cc == 0x6E) {
                printf("[MUTE] Ch %d: XMI Lock (110) = %02X (Tady mùže být ztišení!)\n", channel + 1, val);
            }
            // ... (ostatní CC jako smyèky)

            currentPos += 3;
            continue;
        }

        // ---- 3. NOTE ON (0x90) ----
        if (type == 0x90)
        {
            uint8_t note = *(currentPos + 1);
            uint8_t velocity = *(currentPos + 2);
            
            // Pokud uvidíš Velocity 00, stopa je v dumpu zapsaná, ale nìmá.
            printf("[NOTE ON] Ch %d: Note %d, Vel %d\n", channel + 1, note, velocity);

            currentPos += 3;
            // XMI SPECIFIKUM: Za každou Note On následuje VLQ Duration (délka trvání).
            // Musíme ji pøeskoèit, jinak parser selže na dalším eventu.
            while(currentPos < endPos && (*currentPos & 0x80)) {
                currentPos++;
            }
            currentPos++; 
            continue;
        }

        // ---- 4. META EVENTS (0xFF) ----
        if (status == 0xFF)
        {
            uint8_t metaType = *(currentPos + 1);
            // V tvém souboru se objevuje Sequence Volume 
            if (metaType == 0x50) {
                uint8_t globalVol = *(currentPos + 3);
                printf("[GLOBAL] Master Sequence Volume = %02X (%d)\n", globalVol, globalVol);
                currentPos += 4;
            }
            else if (metaType == 0x2F) {
                printf("[END] End of track\n");
                currentPos += 3;
            }
            else {
                // Pokud je to neznámý meta event (napø. text), musíme skoèit za nìj
                uint8_t len = *(currentPos + 2);
                currentPos += (3 + len);
            }
            continue;
        }

        currentPos++;
    }
}
	/*
	for (int i = 1; i <= m_iNumberOfTracks; i++)//2b4804
		AilInitSequence_95C00(m_hMusicSequence, musicHeader_E3808->str_8.track_10[i - 1].xmiData_0, 0, i);
	*/

	//musicAble_E37FC = true;
	return true;
}

int main()
{
	char musicPath[512];
	FILE* filehandle;
	int datapos;
	int channel = 0;
	int channellplus;
	int finaldrivernumber = 2;
	int16_t driverarray[4];
	std::string cdDataPath="c:/COMPUTES/140/prenos/remc2-dev2/remc2/x64/Debug/CD_Files";
	sprintf_s(musicPath, "%s/SOUND/MUSIC.DAT", cdDataPath.c_str());
	if (fopen_s(&filehandle, musicPath, "rb") != 0)
	if (!filehandle)
		return false;
	fseek(filehandle, 0, 2);
	filelenght = ftell(filehandle);
	fseek(filehandle, filelenght - 4, 0);
	fread((uint8_t*)&datapos, 1, 4, filehandle);
	//DataFileIO::Read(filehandle, (uint8_t*)&datapos, 4);
	fseek(filehandle, datapos, 0);
	fread((uint8_t*)driverarray, 1, 8, filehandle);
	//DataFileIO::Read(filehandle, (uint8_t*)driverarray, 8);
	channellplus = channel + 1;

	if (channellplus <= driverarray[finaldrivernumber])
	{
		fseek(filehandle, (channellplus - 1) << 6, 1);
		if (!LoadMusicTrack(filehandle, finaldrivernumber))
		{
			fclose(filehandle);
			return true;
		}
		fclose(filehandle);
		return false;
	}
	fclose(filehandle);

	std::cin.get();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

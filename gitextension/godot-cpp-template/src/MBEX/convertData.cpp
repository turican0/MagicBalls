#include "convertData.h"
/*
#include "remc2/utilities/DataFileRNC.h"
#include "remc2/engine/Terrain.h"
#include "remc2/engine/ConvertMapInfo.h"
#include "remc2/engine/EventsFunctions.h"
#include "remc2/engine/LevelInit.h"
#include "remc2/engine/Level.h"
#include "remc2/engine/DatTabIndexes.h"
#include "remc2/sub_main.h"
#include "remc2/engine/ReadAndDecompress.h"
#include "remc2/engine/MenusAndIntros.h"
#include "remc2/engine/Basic.h"
#include "remc2/engine/GameUI.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
*/

#include <errno.h>
#include <string.h>

#if defined(_WIN32)
#include <direct.h>
#define MKDIR(path) _wmkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

void MBEXconvertData(String path) {
	MBEXsoundConverts(path + "/sounds");
	MBEXmusicConverts(path + "/musics");
}

void MBEXmusicConverts(String path) {
	for (int i = 1; i <= m_iNumberOfTracks; i++)
	{
		uint8_t *buffer = musicHeader_E3808->str_8.track_10[i - 1].xmiData_0;
		int32_t size = musicHeader_E3808->str_8.track_10[i - 1].xmiSize_8;
		int8_t *filename_c = musicHeader_E3808->str_8.track_10[i - 1].filename_14;
		if (!buffer || size <= 0 || !filename_c) {
			break;
		}
		String filename = String::utf8((const char *)filename_c);
		String full_path = path + "/" + vformat("%03d_%s", i-1, filename);
		if (!make_dir_godot(path)) {
			break;
		}
		Ref<FileAccess> file = FileAccess::open(full_path, FileAccess::WRITE);
		if (!file.is_valid()) {
			break;
		}
		file->store_buffer(buffer, size);
		Error err = file->get_error();
		if (err != OK) {
			UtilityFunctions::push_error(vformat("CHYBA při zápisu do souboru: %s (Error: %d, zapsáno před chybou)", full_path, (int)err));
			break;
		}
		file->flush();
		UtilityFunctions::print(vformat("Úspěšně zapsáno: %s (%d bajtů)", full_path, size));
	}
}

void MBEXsoundConverts(String path) {
	bool soundExists = true;
	int i = 0;
	while (soundExists) {
		soundExists = MBEXsoundConvert(i,path);
		i++;
	}
}

/*
bool make_dir_godot(const String &path) {
	Ref<DirAccess> dir = DirAccess::open("user://");
	if (!dir.is_valid()) {
		return false;
	}
	String relative_path = path;
	if (path.begins_with("user://")) {
		relative_path = path.substr(7);
	}
	Error err = dir->make_dir_recursive(relative_path);
	return err == OK;
}
*/
bool make_dir_godot(const String &path) {
	String root = path.begins_with("user://") ? "user://" : "res://";
	Ref<DirAccess> dir = DirAccess::open(root);
	if (!dir.is_valid()) {
		return false;
	}
	String relative_path = path.substr(root.length());
	Error err = dir->make_dir_recursive(relative_path);
	return err == OK;
}

bool MBEXsoundConvert(int index, String path) {
	bool result = false;
	char *OUT_PATH = (char*)path.utf8().get_data();
	if (!MBLoadSound(index))
		return false;
	bool subSoundExists = true;
	int j = 0;
	while (subSoundExists) {
		uint8_t *buffer = soundIndex_E37A0->str_8.wavs_10[j].wavData_18;
		int32_t size = soundIndex_E37A0->str_8.wavs_10[j].wavSize_26;
		int8_t *filename_c = soundIndex_E37A0->str_8.wavs_10[j].filename_0;
		/*
		if (!soundIndex_E37A0->str_8.wavs_10[j].word_30) {
			j++;
			continue;
		}*/
		if (!buffer || size <= 0 || !filename_c) {
			break;
		}
		String filename = String::utf8((const char *)filename_c);
		//String dir_path = "user://convertdata/sounds";
		String full_path = path + "/" + vformat("%03d_%03d_%s", index, j, filename);
		if (!make_dir_godot(path)) {
			break;
		}
		Ref<FileAccess> file = FileAccess::open(full_path, FileAccess::WRITE);
		if (!file.is_valid()) {
			break;
		}
		/*
		PackedByteArray mutable_buffer;
		mutable_buffer.resize(size);
		memcpy(mutable_buffer.ptrw(), buffer, size);
		file->store_buffer(mutable_buffer.ptr(), size);
		*/
		file->store_buffer(buffer, size);
		Error err = file->get_error();
		if (err != OK) {
			UtilityFunctions::push_error(vformat("CHYBA při zápisu do souboru: %s (Error: %d, zapsáno před chybou)", full_path, (int)err));
			break;
		}
		file->flush();
		UtilityFunctions::print(vformat("Úspěšně zapsáno: %s (%d bajtů)", full_path, size));
		j++;
	}
	if (j == 0)
		return false;
	return true;
}

bool MBLoadSound(uint8_t soundIndex) //265300
{
	FILE *file;
	long sizeOfFile;
	int16_t lastSoundBank[6];
	int32_t lastSoundBankPos;
	uint8_t soundIndex2 = 0;

	std::string soundPath = GetSubDirectoryFile(cdFolder, "SOUND", "SOUND.DAT");
	file = DataFileIO::CreateOrOpenFile(soundPath.c_str(), 512);

	if (file != NULL) {
		DataFileIO::Seek(file, 0, 2);
		sizeOfFile = myftell(file);
		DataFileIO::Seek(file, sizeOfFile - 4, 0);
		DataFileIO::Read(file, (uint8_t *)&lastSoundBankPos, 4);
		DataFileIO::Seek(file, lastSoundBankPos, 0);
		DataFileIO::Read(file, (uint8_t *)lastSoundBank, 12);
		soundFreqType_E37B6 = 822;
		switch (soundFreqType_E37B6) {
			case 800:
				soundIndex2 = 5; //800
				break;
			case 811:
				soundIndex2 = 4; //811
				break;
			case 822:
				soundIndex2 = 3; //822
				break;
			case 1611:
				soundIndex2 = 2; //1611
				break;
			case 1622:
				soundIndex2 = 1; //1622
				break;
			case 1644:
				soundIndex2 = 0; //1644
				break;
		}
		if ((soundIndex + 1) > lastSoundBank[soundIndex2]) {
			DataFileIO::Close(file);
			return false;
		}
		DataFileIO::Seek(file, 96 * soundIndex, 1); //seek to finded sound
		if (!ReadAndDecompressSound(file, soundIndex2)) {
			DataFileIO::Close(file);
			return false;
		}
		DataFileIO::Close(file);
	}
	return true;
}

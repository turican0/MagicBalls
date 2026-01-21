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
	MBEXtexturesConverts(path + "/textures");
}
void MBEXtexturesConverts(String path) {
	MBEXtextureConverts(path + "/day", 256, 608, "BLOCK32.DAT", "PALD-0.DAT");
	MBEXtextureConverts(path + "/night", 256, 608, "BL32N0-0.DAT", "PALN-0.DAT");
	MBEXtextureConverts(path + "/cave", 256, 608, "BL32C0-0.DAT", "PALC-0.DAT");
	MBEXtextureConverts(path + "/final", 256, 608, "BL32F0-0.DAT", "PALF-0.DAT");
}

void MBEXtextureConverts(String path, int width, int height, String texture, String palette) {
	std::string textPath = GetSubDirectoryFile(cdFolder, "DATA", texture.utf8().get_data());
	std::string palettePath = GetSubDirectoryFile(cdFolder, "DATA", palette.utf8().get_data());
	String outPath = path + "/" + texture + ".bmp";
	std::string stdOutPath = outPath.utf8().get_data();

	if (!make_dir_godot(path)) {
		return;
	}

// Načtení dat textury pomocí Godot FileAccess
	Ref<FileAccess> texFile = FileAccess::open(String(textPath.c_str()), FileAccess::READ);
	if (texFile.is_null())
		return;
	PackedByteArray rawPixels = texFile->get_buffer(texFile->get_length());

	// Načtení dat palety
	Ref<FileAccess> palFile = FileAccess::open(String(palettePath.c_str()), FileAccess::READ);
	if (palFile.is_null())
		return;
	PackedByteArray rawPalette = palFile->get_buffer(palFile->get_length());

	// Otevření souboru pro zápis (Godot si s res:// poradí sám)
	Ref<FileAccess> bFile = FileAccess::open(outPath, FileAccess::WRITE);
	if (bFile.is_null())
		return;

	// --- VÝPOČTY ---
	int paddingSize = (4 - (width % 4)) % 4;
	int rowSize = width + paddingSize;
	int pixelDataSize = rowSize * height;
	int paletteSize = 256 * 4;
	int fileSize = 14 + 40 + paletteSize + pixelDataSize;

	// --- BITMAP FILE HEADER (14 bytes) ---
	bFile->store_8('B');
	bFile->store_8('M');
	bFile->store_32(fileSize);
	bFile->store_32(0); // Reserved
	bFile->store_32(14 + 40 + paletteSize); // Offset

	// --- DIB HEADER (40 bytes) ---
	bFile->store_32(40); // Header size
	bFile->store_32(width);
	bFile->store_32(height);
	bFile->store_16(1); // Planes
	bFile->store_16(8); // Bits per pixel
	bFile->store_32(0); // Compression
	bFile->store_32(pixelDataSize);
	bFile->store_32(2835); // X ppm
	bFile->store_32(2835); // Y ppm
	bFile->store_32(256); // Colors used
	bFile->store_32(0); // Important colors

	// --- PALETTE (1024 bytes) ---
	for (int i = 0; i < 256; i++) {
		if (i * 3 + 2 < rawPalette.size()) {
			bFile->store_8(rawPalette[i * 3 + 2]*4); // B
			bFile->store_8(rawPalette[i * 3 + 1]*4); // G
			bFile->store_8(rawPalette[i * 3]*4); // R
			bFile->store_8(0); // Reserved
		} else {
			bFile->store_32(0); // Vyplnění prázdné palety
		}
	}

	// --- PIXEL DATA (Bottom-up) ---
	for (int y = height - 1; y >= 0; y--) {
		int rowStart = y * width;
		if (rowStart < rawPixels.size()) {
			// Vezmeme pod-pole pro aktuální řádek a uložíme ho
			PackedByteArray row = rawPixels.slice(rowStart, rowStart + width);
			bFile->store_buffer(row);
		} else {
			// Padding pokud data chybí
			for (int x = 0; x < width; x++)
				bFile->store_8(0);
		}

		// Zápis paddingu na konci řádku
		for (int p = 0; p < paddingSize; p++) {
			bFile->store_8(0);
		}
	}

	bFile->flush();
	bFile->close();
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

		// Změna přípony na .mid (nebo přidej .mid k názvu, pokud chceš zachovat původní)
		String midi_filename = filename.get_basename() + ".mid"; // nebo jen filename + ".mid"

		String full_path = path + "/" + vformat("%03d_%s", i - 1, midi_filename);

		if (!make_dir_godot(path)) {
			break;
		}

		// Převod XMI → MIDI
		size_t midi_size = 0;
		unsigned char *midi_buffer = TranscodeXmiToMid(buffer, size, &midi_size);

		if (!midi_buffer || midi_size == 0) {
			UtilityFunctions::push_error(vformat("CHYBA při převodu XMI na MIDI pro track %d (%s)", i - 1, filename));
			// Pokud převod selže, volitelně uložit původní XMI pro debugging
			// free(midi_buffer); // pokud bylo alokováno
			break;
		}

		Ref<FileAccess> file = FileAccess::open(full_path, FileAccess::WRITE);
		if (!file.is_valid()) {
			free(midi_buffer); // uvolnit alokovanou paměť
			break;
		}

		file->store_buffer(midi_buffer, midi_size);

		Error err = file->get_error();
		if (err != OK) {
			UtilityFunctions::push_error(vformat("CHYBA při zápisu do souboru: %s (Error: %d)", full_path, (int)err));
			free(midi_buffer);
			break;
		}

		file->flush();
		file->close(); // dobrý zvyk

		free(midi_buffer); // uvolnit paměť vrácenou TranscodeXmiToMid

		UtilityFunctions::print(vformat("Úspěšně zapsáno MIDI: %s (%d bajtů z %d bajtů XMI)", full_path, midi_size, size));
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

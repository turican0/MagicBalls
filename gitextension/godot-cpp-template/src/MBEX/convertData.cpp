#include "convertData.h"
#include "../lib/stb_image_write.h"
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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <direct.h>
#define MKDIR(path) _wmkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

using namespace godot;
namespace fs = std::filesystem;

/*
struct BMPData {
	int width = 0;
	int height = 0;
	int channels = 0;
	std::vector<unsigned char> pixels;
};

void save_bmp_godot(String p_path, int p_width, int p_height, int p_channels, const unsigned char *p_data) {
	// Výpočet velikosti řádku s paddingem na 4 bajty (standard BMP)
	int row_size = ((p_width * p_channels + 3) & ~3);
	int image_size = row_size * p_height;
	int file_size = 54 + image_size;

	Ref<FileAccess> bFile = FileAccess::open(p_path, FileAccess::WRITE);
	if (bFile.is_null()) {
		// Chyba při otevírání
		return;
	}

	// --- BITMAP FILE HEADER (14 bajtů) ---
	bFile->store_16(0x4D42); // bfType: "BM"
	bFile->store_32(file_size); // bfSize
	bFile->store_16(0); // bfReserved1
	bFile->store_16(0); // bfReserved2
	bFile->store_32(54); // bfOffBits (hlavičky mají dohromady 54 bajtů)

	// --- BITMAP INFO HEADER (40 bajtů) ---
	bFile->store_32(40); // biSize
	bFile->store_32(p_width); // biWidth
	bFile->store_32(-p_height); // biHeight (záporné = shora dolů)
	bFile->store_16(1); // biPlanes
	bFile->store_16(p_channels * 8); // biBitCount (24 nebo 32)
	bFile->store_32(0); // biCompression (BI_RGB = 0)
	bFile->store_32(image_size); // biSizeImage
	bFile->store_32(2835); // biXPelsPerMeter (72 DPI)
	bFile->store_32(2835); // biYPelsPerMeter
	bFile->store_32(0); // biClrUsed
	bFile->store_32(0); // biClrImportant

	// --- PIXEL DATA ---
	for (int y = 0; y < p_height; ++y) {
		// Zapíšeme data řádku
		const unsigned char *row_ptr = &p_data[y * p_width * p_channels];
		bFile->store_buffer(row_ptr, p_width * p_channels);

		// Doplnění paddingu, aby délka řádku byla násobkem 4
		int padding_bytes = row_size - (p_width * p_channels);
		for (int p = 0; p < padding_bytes; ++p) {
			bFile->store_8(0);
		}
	}

	bFile->flush();
	bFile->close();
}

BMPData load_bmp_godot(String p_path) {
	BMPData result;
	Ref<FileAccess> bFile = FileAccess::open(p_path, FileAccess::READ);

	if (bFile.is_null())
		return result;

	bFile->get_16(); // "BM"
	bFile->seek(10);
	uint32_t data_offset = bFile->get_32();
	uint32_t header_size = bFile->get_32();
	result.width = bFile->get_32();
	int32_t raw_height = bFile->get_32();
	result.height = std::abs(raw_height);
	bFile->get_16(); // Planes
	uint16_t bit_count = bFile->get_16();

	// Vynutíme 3 kanály pro výstup (převod z palety na RGB)
	result.channels = 3;

	// --- NAČTENÍ PALETY (pouze pro 8-bit) ---
	std::vector<uint8_t> color_table;
	if (bit_count == 8) {
		// Paleta začíná hned po Info Headeru (v našem případě na offsetu 14 + header_size)
		bFile->seek(14 + header_size);
		// Standardní 8-bit BMP má 256 barev (každá má 4 bajty: B, G, R, Reserved)
		color_table.resize(256 * 4);
		PackedByteArray pal_buffer = bFile->get_buffer(256 * 4);
		for (int i = 0; i < pal_buffer.size(); ++i)
			color_table[i] = pal_buffer[i];
	} else if (bit_count < 24) {
		UtilityFunctions::print("Chyba: Podporujeme jen 8, 24 nebo 32 bit BMP.");
		return result;
	}

	result.pixels.resize((size_t)result.width * result.height * 3);

	// Padding v ZDROJOVÉM souboru (pro 8-bit je to šířka v bajtech zarovnaná na 4)
	int src_channels = bit_count / 8;
	int row_size_in_file = ((result.width * src_channels + 3) & ~3);
	int file_padding = row_size_in_file - (result.width * src_channels);

	bFile->seek(data_offset);

	for (int y = 0; y < result.height; ++y) {
		int target_y = (raw_height > 0) ? (result.height - 1 - y) : y;
		PackedByteArray row_data = bFile->get_buffer(result.width * src_channels);

		for (int x = 0; x < result.width; ++x) {
			uint64_t out_pos = (uint64_t)(target_y * result.width + x) * 3;

			if (bit_count == 8) {
				// Převod indexu na RGB pomocí palety
				uint8_t index = row_data[x];
				result.pixels[out_pos + 0] = color_table[index * 4 + 0]; // B
				result.pixels[out_pos + 1] = color_table[index * 4 + 1]; // G
				result.pixels[out_pos + 2] = color_table[index * 4 + 2]; // R
			} else {
				// Klasické 24-bit/32-bit (jen kopírujeme první 3 kanály)
				result.pixels[out_pos + 0] = row_data[x * src_channels + 0];
				result.pixels[out_pos + 1] = row_data[x * src_channels + 1];
				result.pixels[out_pos + 2] = row_data[x * src_channels + 2];
			}
		}
		bFile->get_buffer(file_padding); // Přeskočit padding v souboru
	}

	bFile->close();
	return result;
}
*/

void MBEXconvertData(String path, String path2) {
	UtilityFunctions::print("MBEXconvertData START");
	UtilityFunctions::print("MBEXconvertData path: ", path);
	UtilityFunctions::print("MBEXconvertData path2: ", path2);
	UtilityFunctions::print("MBEXconvertData calling MBEXfontsConverts...");
	MBEXfontsConverts(path + "/fonts");
	UtilityFunctions::print("MBEXconvertData MBEXfontsConverts done");
	UtilityFunctions::print("MBEXconvertData calling MBEXsoundConverts...");
	MBEXsoundConverts(path + "/sounds");
	UtilityFunctions::print("MBEXconvertData MBEXsoundConverts done");
	UtilityFunctions::print("MBEXconvertData calling MBEXmusicConverts g...");
	MBEXmusicConverts(path + "/musics", 'g');
	UtilityFunctions::print("MBEXconvertData MBEXmusicConverts g done");
	UtilityFunctions::print("MBEXconvertData calling MBEXmusicConverts r...");
	MBEXmusicConverts(path + "/musics", 'r');
	UtilityFunctions::print("MBEXconvertData MBEXmusicConverts r done");
	UtilityFunctions::print("MBEXconvertData calling MBEXmusicConverts f...");
	MBEXmusicConverts(path + "/musics", 'f');
	UtilityFunctions::print("MBEXconvertData MBEXmusicConverts f done");
	UtilityFunctions::print("MBEXconvertData calling MBEXmusicConverts w...");
	MBEXmusicConverts(path + "/musics", 'w');
	UtilityFunctions::print("MBEXconvertData MBEXmusicConverts w done");
	UtilityFunctions::print("MBEXconvertData calling MBEXtexturesConverts...");
	MBEXtexturesConverts(path + "/textures");
	UtilityFunctions::print("MBEXconvertData MBEXtexturesConverts done");
	UtilityFunctions::print("MBEXconvertData calling MBEXgraphicsConverts...");
	MBEXgraphicsConverts(path + "/HSPR");
	UtilityFunctions::print("MBEXconvertData MBEXgraphicsConverts done");
	UtilityFunctions::print("MBEXconvertData calling MBEXwebConverts...");
	MBEXwebConverts(path + "/web");
	UtilityFunctions::print("MBEXconvertData MBEXwebConverts done");
	UtilityFunctions::print("MBEXconvertData calling MBEXsmatsConverts...");
	MBEXsmatsConverts(path + "/smat");
	UtilityFunctions::print("MBEXconvertData MBEXsmatsConverts done");
	UtilityFunctions::print("MBEXconvertData calling MBEXhscreenConverts...");
	MBEXhscreenConverts(path + "/HSCREEN");
	UtilityFunctions::print("MBEXconvertData MBEXhscreenConverts done");
	UtilityFunctions::print("MBEXconvertData calling MBEXhtablesConverts...");
	MBEXhtablesConverts(path + "/TABLES");
	UtilityFunctions::print("MBEXconvertData MBEXhtablesConverts done");
	UtilityFunctions::print("MBEXconvertData calling MBEXcdrConverts...");
	MBEXcdrConverts(path + "/CDR");
	UtilityFunctions::print("MBEXconvertData MBEXcdrConverts done");
	UtilityFunctions::print("MBEXconvertData calling MBEXpointersConverts...");
	MBEXpointersConverts(path + "/POINTERS");
	UtilityFunctions::print("MBEXconvertData MBEXpointersConverts done");
	UtilityFunctions::print("MBEXconvertData calling MBEXgtdConverts...");
	MBEXgtdConverts(path + "/GTD");
	UtilityFunctions::print("MBEXconvertData MBEXgtdConverts done");
	UtilityFunctions::print("MBEXconvertData calling MBEXextractLang...");
	MBEXextractLang(path + "/language", "res://hidata/language/", "/LANGUAGE/");
	UtilityFunctions::print("MBEXconvertData MBEXextractLang done");
	UtilityFunctions::print("MBEXconvertData calling MBEXtmapsConverts...");
	MBEXtmapsConverts(path + "/TMAPS");
	UtilityFunctions::print("MBEXconvertData MBEXtmapsConverts done");
	UtilityFunctions::print("MBEXconvertData END");
}

void MBEXextractLang(String path, String langPath, String cdLangPath) {
	if (!make_dir_godot(path)) {
		return;
	}
	String cdFullLangPath = String(cdFolder.c_str()) + cdLangPath;
	Ref<DirAccess> cleanup_dir = DirAccess::open(cdFullLangPath);
	if (cleanup_dir.is_valid()) {
		cleanup_dir->list_dir_begin();
		String f_name = cleanup_dir->get_next();

		while (f_name != "") {
			if (f_name != "." && f_name != "..") {
				cleanup_dir->remove(f_name);
			}
			f_name = cleanup_dir->get_next();
		}
		cleanup_dir->list_dir_end();
	}
	Ref<DirAccess> dir = DirAccess::open(langPath);
	if (dir.is_null()) {
		UtilityFunctions::printerr("MBEX: Failed to open source folder: ", langPath);
		return;
	}
	dir->list_dir_begin();
	String file_name = dir->get_next();
	while (file_name != "") {
		if (file_name != "." && file_name != "..") {
			if (!dir->current_is_dir()) {
				String source_file = langPath.path_join(file_name);
				String dest_file = cdFullLangPath.path_join(file_name);

				Error err = dir->copy(source_file, dest_file);

				if (err != OK) {
					UtilityFunctions::printerr("MBEX: Error while copying ", file_name);
				}
			}
		}
		file_name = dir->get_next();
	}
	dir->list_dir_end();

	char dataPath[MAX_PATH];
	sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.palette_17DE38x, 0xB2947, 768);

	dir = DirAccess::open(langPath);
	if (dir.is_valid()) {
		dir->list_dir_begin();
		String f_name = dir->get_next();
		while (f_name != "") {
			if (f_name != "." && f_name != "..") {
				if (f_name.begins_with("l") || f_name.begins_with("L")) {
					String full_path = langPath.path_join(f_name);
					CharString full_pathUtf = full_path.utf8();
					Ref<FileAccess> file = FileAccess::open(full_path, FileAccess::READ);
					if (file.is_valid()) {
						uint64_t file_len = file->get_length();
						PackedByteArray data = file->get_buffer(file_len);
						memcpy(x_DWORD_E9C38_smalltit, data.ptr(), file_len);
						MBEXsaveBitmapCrop(path, (char *)f_name.utf8().get_data(), 90, 53, x_DWORD_E9C38_smalltit + 1, (TColor *)x_DWORD_17DE38str.palette_17DE38x, 2);
						UtilityFunctions::print("File read and processed: ", f_name);
					} else {
						UtilityFunctions::printerr("Failed to open file: ", full_path);
					}
				}
			}
			f_name = dir->get_next();
		}
		dir->list_dir_end();
	}
}

//TABLES-ok
//SCREENS\HSCREEN0.DAT-later
//BUTTON-ok noise
//CLR C/D/N-ok
//POINTERS-ok
//GTD-ok
//TITBASF-ok

void MBEXsmatConverts(String path, int inWidth, int inHeight, String texture, String palette) {
	std::string textPath = GetSubDirectoryFile(cdFolder.c_str(), "DATA", texture.utf8().get_data());
	std::string palettePath = GetSubDirectoryFile(cdFolder.c_str(), "DATA", palette.utf8().get_data());
	String outPath = path + "/" + texture + ".png";
	std::string stdOutPath = outPath.utf8().get_data();

	if (!make_dir_godot(path)) {
		return;
	}

	PackedByteArray palette_data;
	Ref<FileAccess> palFile = FileAccess::open(String(palettePath.c_str()), FileAccess::READ);
	if (palFile.is_valid()) {
		palette_data = palFile->get_buffer(palFile->get_length());
		palFile->close();
	}

	std::vector<uint8_t> palette_final;
	if (palette_data.size() < 768 && !palette_data.is_empty()) {
		// Paleta je komprimovaná RNC
		std::vector<uint8_t> pal_dst(2048); // Paleta bývá malá, 2KB stačí
		int pal_dec_size = DataFileRNC::Decompress((uint8_t *)palette_data.ptr(), pal_dst.data());
		if (pal_dec_size >= 768) {
			palette_final.assign(pal_dst.begin(), pal_dst.begin() + pal_dec_size);
		}
	} else {
		// Paleta je už v surovém stavu
		palette_final.assign(palette_data.ptr(), palette_data.ptr() + palette_data.size());
	}

	if (palette_final.size() < 768) {
		UtilityFunctions::print("Error: The palette could not be decompressed or is damaged.");
		return;
	}

	PackedByteArray img_data;
	Ref<FileAccess> imgFile = FileAccess::open(String(textPath.c_str()), FileAccess::READ);
	if (imgFile.is_valid()) {
		img_data = imgFile->get_buffer(imgFile->get_length());
		imgFile->close();
	}
	std::vector<uint8_t> img_dst(5000000);
	std::vector<uint8_t> img_final;
	int img_dec_size = DataFileRNC::Decompress((uint8_t *)img_data.ptr(), img_dst.data());
	img_final.assign(img_dst.begin(), img_dst.begin() + img_dec_size);

	int data_start_offset = 0;
	int expected_pixels = inWidth * inHeight;
	if (img_dec_size < data_start_offset + expected_pixels) {
		UtilityFunctions::print("Error: Data does not contain enough pixels for dimensions: ", inWidth, "x", inHeight);
		return;
	}
	int p_channels = 3;
	std::vector<unsigned char> rgb_data((size_t)inWidth * inHeight * p_channels);
	const uint8_t *indices = img_final.data() + data_start_offset;
	for (int i = 0; i < inWidth * inHeight; ++i) {
		uint8_t index = indices[i];
		int pal_idx = index * 3;

		rgb_data[i * 3 + 0] = palette_final[pal_idx + 0]*4; // Blue
		rgb_data[i * 3 + 1] = palette_final[pal_idx + 1]*4; // Green
		rgb_data[i * 3 + 2] = palette_final[pal_idx + 2]*4; // Red
	}

	Ref<FileAccess> f = FileAccess::open(outPath, FileAccess::WRITE);
	if (f.is_valid()) {
		stbi_write_png_to_func(
				[](void *context, void *data, int size) {
					FileAccess *fa = static_cast<FileAccess *>(context);
					fa->store_buffer(static_cast<const uint8_t *>(data), size);
				},
				(void *)f.ptr(),
				inWidth,
				inHeight,
				p_channels,
				rgb_data.data(),
				inWidth * p_channels);

		f->flush();
		f->close();
		UtilityFunctions::print("Saved PNG: ", outPath, " (", inWidth, "x", inHeight, ")");
	} else {
		UtilityFunctions::printerr("Failed to open file for writing: ", outPath);
	}
}

#pragma pack(1)
typedef struct {
	uint32_t data;
	uint8_t width_4;
	uint8_t height_5;
} bitmap_pos_struct_tm;
#pragma pack(pop)

void MBEXgraphicConverts(String path, String texture, String palette) {
	std::string datPath = GetSubDirectoryFile(cdFolder.c_str(), "DATA", texture.utf8().get_data());
	std::string tabPath = datPath.substr(0, datPath.length() - 3) + "TAB";
	std::string palettePath = GetSubDirectoryFile(cdFolder.c_str(), "DATA", palette.utf8().get_data());	

	if (!make_dir_godot(path)) {
		return;
	}

	PackedByteArray palette_data;
	Ref<FileAccess> palFile = FileAccess::open(String(palettePath.c_str()), FileAccess::READ);
	if (palFile.is_valid()) {
		palette_data = palFile->get_buffer(palFile->get_length());
		palFile->close();
	}

	std::vector<uint8_t> palette_final;
	if (palette_data.size() < 768 && !palette_data.is_empty()) {
		std::vector<uint8_t> pal_dst(2048);
		int pal_dec_size = DataFileRNC::Decompress((uint8_t *)palette_data.ptr(), pal_dst.data());
		if (pal_dec_size >= 768) {
			palette_final.assign(pal_dst.begin(), pal_dst.begin() + pal_dec_size);
		}
	} else {
		palette_final.assign(palette_data.ptr(), palette_data.ptr() + palette_data.size());
	}

	if (palette_final.size() < 768) {
		UtilityFunctions::print("Error: The palette could not be decompressed or is damaged.");
		return;
	}

	PackedByteArray fileDat_data;
	Ref<FileAccess> datFile = FileAccess::open(String(datPath.c_str()), FileAccess::READ);
	if (datFile.is_valid()) {
		fileDat_data = datFile->get_buffer(datFile->get_length());
		datFile->close();
	}
	//std::vector<uint8_t> dat_dst(5000000);
	std::vector<uint8_t> dat_final;
	/*
	int dat_dec_size = DataFileRNC::Decompress((uint8_t *)fileDat_data.ptr(), dat_dst.data());
	dat_final.assign(dat_dst.begin(), dat_dst.begin() + dat_dec_size);
	*/

	dat_final.assign(fileDat_data.ptr(), fileDat_data.ptr() + fileDat_data.size());

	PackedByteArray fileTab_data;
	
	Ref<FileAccess> tabFile = FileAccess::open(String(tabPath.c_str()), FileAccess::READ);
	if (tabFile.is_valid()) {
		fileTab_data = tabFile->get_buffer(tabFile->get_length());
		tabFile->close();
	}
	//std::vector<uint8_t> tab_dst(5000000);
	std::vector<uint8_t> tab_final;
	/*
	int tab_dec_size = DataFileRNC::Decompress((uint8_t *)fileTab_data.ptr(), tab_dst.data());
	tab_final.assign(tab_dst.begin(), tab_dst.begin() + tab_dec_size);
	*/
	tab_final.assign(fileTab_data.ptr(), fileTab_data.ptr() + fileTab_data.size());

	bitmap_pos_struct_tm *contentTMAPStab = (bitmap_pos_struct_tm *)tab_final.data();
	uint8_t *contentTMAPSdat = (uint8_t*)dat_final.data();

	int count = fileTab_data.size() / sizeof(bitmap_pos_struct2_t);

	bool success = true;
	for (int index = 0; index < count; index++) {
		int begin = contentTMAPStab[index].data;
		int end = contentTMAPStab[index + 1].data;
		uint8_t* stmpdat = &contentTMAPSdat[begin];
		int width = contentTMAPStab[index].width_4;
		int height = contentTMAPStab[index].height_5;
		int size = end - begin;

		bitmap_pos_struct_t a3;
		a3.data = (uint8 *)contentTMAPStab[index].data;
		a3.width_4 = contentTMAPStab[index].width_4;
		a3.height_5 = contentTMAPStab[index].height_5;
		//GameBitmapDrawTransparentBitmap_2DE80(0, 0, a3, 0);//20ee80

		static uint8_t buf1[640 * 480];
		static uint8_t buf2[640 * 480];
		memset(pdwScreenBuffer_351628, 10, 640*480);
		GameBitmap::DrawMenuGraphic(contentTMAPStab[index].width_4, contentTMAPStab[index].height_5, 1, stmpdat, pdwScreenBuffer_351628);
		memcpy(buf1, pdwScreenBuffer_351628, 640 * 480);

		memset(pdwScreenBuffer_351628, 20, 640 * 480);
		GameBitmap::DrawMenuGraphic(contentTMAPStab[index].width_4, contentTMAPStab[index].height_5, 1, stmpdat, pdwScreenBuffer_351628);
		memcpy(buf2, pdwScreenBuffer_351628, 640 * 480);

		//--------------------------
		int p_channels = 4;
		std::vector<unsigned char> rgba_data((size_t)width * height * p_channels);
		for (int i = 0; i < width * height; i++) {
			uint8_t idx1 = buf1[i];
			uint8_t idx2 = buf2[i];

			bool opaque = !(idx1 != idx2);

			uint8_t colorIndex = idx1;
			int pal_idx = colorIndex * 3;
			int dest_idx = i * 4;

			rgba_data[dest_idx + 0] = palette_final[pal_idx + 0] * 4; // Red
			rgba_data[dest_idx + 1] = palette_final[pal_idx + 1] * 4; // Green
			rgba_data[dest_idx + 2] = palette_final[pal_idx + 2] * 4; // Blue
			rgba_data[dest_idx + 3] = opaque ? 255 : 0;
		}
		char buf[16];
		snprintf(buf, sizeof(buf), "%03d", (int)index);
		String outPath = path + "/" + texture + "_" + String(buf) + ".png";
		// Otevření souboru přes Godot API
		Ref<FileAccess> f = FileAccess::open(outPath, FileAccess::WRITE);
		if (f.is_valid()) {
			// Volání STB s Lambda funkcí přímo v argumentu
			stbi_write_png_to_func(
					[](void *context, void *data, int size) {
						// Context je náš FileAccess*
						FileAccess *fa = static_cast<FileAccess *>(context);
						fa->store_buffer(static_cast<const uint8_t *>(data), size);
					},
					(void *)f.ptr(), // Předání ukazatele na FileAccess objekt
					width,
					height,
					p_channels,
					rgba_data.data(),
					width * p_channels);

			f->flush();
			f->close();
			UtilityFunctions::print("PNG saved (lambda): ", outPath);
		}
	}
}

void MBEXtextureConvertsP(String path, int inWidth, int inHeight, String texture) {
	MBEXtextureConverts(path + "/PALC", inWidth, inHeight, texture, "PALC-0.DAT", false);
	MBEXtextureConverts(path + "/PALD", inWidth, inHeight, texture, "PALD-0.DAT", false);
	MBEXtextureConverts(path + "/PALF", inWidth, inHeight, texture, "PALF-0.DAT", false);
	MBEXtextureConverts(path + "/PALN", inWidth, inHeight, texture, "PALN-0.DAT", false);
	MBEXtextureConverts(path + "/PALLOGO", inWidth, inHeight, texture, "PALLOGO.DAT", false);
	MBEXtextureConverts(path + "/SMATITLE", inWidth, inHeight, texture, "PALETTE.DAT", false);
	MBEXtextureConverts(path + "/PALTIT3", inWidth, inHeight, texture, "PALTIT3.DAT", false);

	MBEXtextureConverts(path + "/SMALTITP", inWidth, inHeight, texture, "SMALTIT.PAL", false);
	MBEXtextureConverts(path + "/SMATITL2P", inWidth, inHeight, texture, "SMATITL2.PAL", false);
	MBEXtextureConverts(path + "/SMATITLEP", inWidth, inHeight, texture, "SMATITLE.PAL", false);
}

void MBEXhbuttonsConverts(String path) {
	MBEXgraphicConverts(path + "", "BUTTON.DAT", "PALD-0.DAT");
	MBEXtextureConvertsP(path + "", 256, 256, "BUTTON.DAT");
}

void MBEXgtdConverts(String path) {
	MBEXtextureConverts(path + "", 256, 256, "GTDEFD.DAT", "PALD-0.DAT", false);
	MBEXtextureConverts(path + "", 256, 256, "GTDEFN.DAT", "PALN-0.DAT", false);
	MBEXtextureConverts(path + "", 256, 256, "GTDEFC.DAT", "PALC-0.DAT", false);
}

void MBEXpointersConverts(String path) {
	MBEXgraphicConverts(path + "/PALLOGO", "POINTERS.DAT", "PALLOGO.DAT");
	MBEXgraphicConverts(path + "/PALETTE", "POINTERS.DAT", "PALETTE.DAT");
	MBEXgraphicConverts(path + "/PALTIT3", "POINTERS.DAT", "PALTIT3.DAT");
	MBEXgraphicConverts(path + "/SMATITL2", "POINTERS.DAT", "SMATITL2.PAL");
	MBEXgraphicConverts(path + "/SMATITLE", "POINTERS.DAT", "SMATITLE.PAL");
}

void MBEXcdrConverts(String path) {
	MBEXtextureConverts(path + "/day", 16, 256, "CLRD-0.DAT", "PALD-0.DAT", false);
	MBEXtextureConverts(path + "/night", 16, 256, "CLRN-0.DAT", "PALN-0.DAT", false);
	MBEXtextureConverts(path + "/cave", 16, 256, "CLRC-0.DAT", "PALC-0.DAT", false);
}

void MBEXhtablesConverts(String path) {
	MBEXtextureConverts(path + "/day", 256, 326, "TABLESD.DAT", "PALD-0.DAT", false);
	MBEXtextureConverts(path + "/night", 256, 326, "TABLESN.DAT", "PALN-0.DAT", false);
	MBEXtextureConverts(path + "/cave", 256, 326, "TABLESC.DAT", "PALC-0.DAT", false);
	//MBEXtextureConverts(path + "/none", 256, 326, "TABLES.DAT", "PALF-0.DAT", false);-only noise
}

void MBEXsaveSprite(String path, int i, bitmap_pos_struct_t bitmap, TColor* palette, bool alpha) {
	int inWidth = bitmap.width_4;
	int inHeight = bitmap.height_5;
	uint8_t* data = bitmap.data;

	if (data == nullptr || inWidth <= 0 || inHeight <= 0 || inWidth > 4096 || inHeight > 4096)
	return;

	char pal[768];
	for (int i=0;i<256;i++)
		{
		pal[i * 3 + 0] = palette[i].red; // R
		pal[i * 3 + 1] = palette[i].green; // G
		pal[i * 3 + 2] = palette[i].blue; // B
	}
	String outPath = path + "/" + vformat("%03d",i) + ".png";
	std::string stdOutPath = outPath.utf8().get_data();

	if (!make_dir_godot(path)) {
		return;
	}
	std::vector<uint8_t> palette_final(768, 0);
	memcpy(palette_final.data(), pal, 768);
	memset(pdwScreenBuffer_351628, 0, 640*480);
	GameBitmap::DrawMenuGraphic(inWidth, inHeight, 1, data, pdwScreenBuffer_351628);

	//--------------------------
	int p_channels = 4;
	std::vector<unsigned char> rgba_data((size_t)inWidth * inHeight * p_channels);
	const uint8_t *indices = pdwScreenBuffer_351628;

	bool alpha2 = (indices[0] == 0 ||
			indices[inWidth - 1] == 0 ||
			indices[(inHeight - 1) * inWidth] == 0 ||
			indices[inWidth * inHeight - 1] == 0);
	if (!alpha2)
		alpha = false;

	for (int i = 0; i < inWidth * inHeight; i++) {
		uint8_t index = indices[i];
		int pal_idx = index * 3;
		int dest_idx = i * 4;
		rgba_data[dest_idx + 0] = palette_final[pal_idx + 0] * 4; // Red
		rgba_data[dest_idx + 1] = palette_final[pal_idx + 1] * 4; // Green
		rgba_data[dest_idx + 2] = palette_final[pal_idx + 2] * 4; // Blue
		rgba_data[dest_idx + 3] = 255;
		if (alpha) {
			if (index == 0) {
				rgba_data[dest_idx + 3] = 0; // transparent
			}
		}
	}
	Ref<FileAccess> f = FileAccess::open(outPath, FileAccess::WRITE);
	if (f.is_valid()) {
		stbi_write_png_to_func(
				[](void *context, void *data, int size) {
					FileAccess *fa = static_cast<FileAccess *>(context);
					fa->store_buffer(static_cast<const uint8_t *>(data), size);
				},
				(void *)f.ptr(),
				inWidth,
				inHeight,
				p_channels,
				rgba_data.data(),
				inWidth * p_channels);

		f->flush();
		f->close();
		UtilityFunctions::print("PNG ulozeno (lambda): ", outPath);
	}
}

void MBEXsaveBitmapCrop(String path, char *name, int width, int height, uint8_t *data, TColor *palette, int cropXmin) {
	int inWidth = width;
	int inHeight = height;
	char pal[768];
	for (int i = 0; i < 256; i++) {
		pal[i * 3 + 0] = palette[i].red; // R
		pal[i * 3 + 1] = palette[i].green; // G
		pal[i * 3 + 2] = palette[i].blue; // B
	}
	String outPath = path + "/" + name + ".png";
	std::string stdOutPath = outPath.utf8().get_data();
	if (!make_dir_godot(path)) {
		return;
	}
	std::vector<uint8_t> palette_final(768, 0);
	memcpy(palette_final.data(), pal, 768);
	int p_channels = 4;
	std::vector<unsigned char> rgba_data((size_t)(inWidth - cropXmin) * inHeight * p_channels);
	const uint8_t *indices = data;
	for (int y = 0; y < inHeight; y++)
	for (int x = 0; x < inWidth; x++)
	{
			if (x - cropXmin>=0) {
				uint8_t index = indices[y * inWidth + x];
				int pal_idx = index * 3;
				int dest_idx = (y * (inWidth - cropXmin) + (x - cropXmin)) * 4;
				rgba_data[dest_idx + 0] = palette_final[pal_idx + 0] * 4; // Red
				rgba_data[dest_idx + 1] = palette_final[pal_idx + 1] * 4; // Green
				rgba_data[dest_idx + 2] = palette_final[pal_idx + 2] * 4; // Blue
				rgba_data[dest_idx + 3] = 255;
			}
	}
	Ref<FileAccess> f = FileAccess::open(outPath, FileAccess::WRITE);
	if (f.is_valid()) {
		stbi_write_png_to_func(
				[](void *context, void *data, int size) {
					FileAccess *fa = static_cast<FileAccess *>(context);
					fa->store_buffer(static_cast<const uint8_t *>(data), size);
				},
				(void *)f.ptr(),
				inWidth - cropXmin,
				inHeight,
				p_channels,
				rgba_data.data(),
				(inWidth - cropXmin) * p_channels);

		f->flush();
		f->close();
		UtilityFunctions::print("PNG ulozeno (lambda): ", outPath);
	}
}

void MBEXsaveBitmap(String path, char *name, int width, int height, uint8_t *data, TColor *palette, bool alpha, int alphaIndex) {
	int inWidth = width;
	int inHeight = height;
	char pal[768];
	for (int i = 0; i < 256; i++) {
		pal[i * 3 + 0] = palette[i].red; // R
		pal[i * 3 + 1] = palette[i].green; // G
		pal[i * 3 + 2] = palette[i].blue; // B
	}
	String outPath = path + "/" + name + ".png";
	std::string stdOutPath = outPath.utf8().get_data();
	if (!make_dir_godot(path)) {
		return;
	}
	std::vector<uint8_t> palette_final(768, 0);
	memcpy(palette_final.data(), pal, 768);
	int p_channels = 4;
	std::vector<unsigned char> rgba_data((size_t)inWidth * inHeight * p_channels);
	const uint8_t *indices = data;
	bool alpha2 = (indices[0] == 0 ||
			indices[width - 1] == 0 ||
			indices[(height - 1) * width] == 0 ||
			indices[width * height - 1] == 0);
	if (!alpha2)
		alpha = false;
	for (int i = 0; i < inWidth * inHeight; ++i) {
		uint8_t index = indices[i];
		int pal_idx = index * 3;
		int dest_idx = i * 4;
		rgba_data[dest_idx + 0] = palette_final[pal_idx + 0] * 4; // Red
		rgba_data[dest_idx + 1] = palette_final[pal_idx + 1] * 4; // Green
		rgba_data[dest_idx + 2] = palette_final[pal_idx + 2] * 4; // Blue
		rgba_data[dest_idx + 3] = 255;
		if (alpha) {
			if (index == alphaIndex) {
				rgba_data[dest_idx + 3] = 0; // transparent
			}
		}
	}
	Ref<FileAccess> f = FileAccess::open(outPath, FileAccess::WRITE);
	if (f.is_valid()) {
		stbi_write_png_to_func(
				[](void *context, void *data, int size) {
					FileAccess *fa = static_cast<FileAccess *>(context);
					fa->store_buffer(static_cast<const uint8_t *>(data), size);
				},
				(void *)f.ptr(),
				inWidth,
				inHeight,
				p_channels,
				rgba_data.data(),
				inWidth * p_channels);

		f->flush();
		f->close();
		UtilityFunctions::print("PNG ulozeno (lambda): ", outPath);
	}
}

void MBEXhscreenConverts(String path) {
	//x_WORD_180660_VGA_type_resolution = 1;
	char dataPath[MAX_PATH];
	char pathBuffer[MAX_PATH];
	x_DWORD_17DE38str.x_DWORD_17DE54 = &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[301787];
	x_DWORD_17DE38str.x_DWORD_17DEC0 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[308527];
	x_DWORD_17DE38str.x_DWORD_17DEC4 = (bitmap_pos_struct2_t *)&x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[310159];
	sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SCREENS/HSCREEN0.DAT");
	sub_7AA70_load_and_decompres_dat_file(dataPath, &x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226[301787], 0x164FCD, 0x35C);
	sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DEC0, 0x165329, 0x224);
	sub_7AA70_load_and_decompres_dat_file(0, 0, 0, 0);
	sub_7AA70_load_and_decompres_dat_file(dataPath, *xadatapald0dat2.colorPalette_var28, 0x17C118, 0x300);

	if (x_WORD_180660_VGA_type_resolution & 1)
		sub_98709_create_index_dattab_power(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);
	else
		sub_9874D_create_index_dattab(x_DWORD_17DE38str.x_DWORD_17DEC0, x_DWORD_17DE38str.x_DWORD_17DEC4, x_DWORD_17DE38str.x_DWORD_17DE54, xy_DWORD_17DEC0_spritestr);
	for (int i = 0; i < 256; i++)
		MBEXsaveSprite(path + "/INTRO-B", i, xy_DWORD_17DEC0_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);
	//x_WORD_180660_VGA_type_resolution = 8;

	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);//4,6,7,12,14,15
	for (int i = 0; i <= 312; i++)
		MBEXsaveSprite(path + "/4", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28,true);
	for (int i = 0; i < 256; i++)
		MBEXsaveSprite(path + "/4-B", i, xy_DWORD_17DEC0_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);

	MBEXsaveBitmap(path, "menuBackground", 640, 480, x_DWORD_E9C38_smalltit, (TColor *)*xadatapald0dat2.colorPalette_var28, false,0);

	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 6); //4,6,7,12,14,15
	for (int i = 0; i <= 312; i++)
		if (i>=285&&i<=304)
			MBEXsaveSprite(path + "/6", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, false);
		else
			MBEXsaveSprite(path + "/6", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);
	for (int i = 0; i < 256; i++)
		MBEXsaveSprite(path + "/6-B", i, xy_DWORD_17DEC0_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);
	for (int i = 0; i < 256; i++)
		MBEXsaveSprite(path + "/6-C", i, xy_DWORD_17DEC8_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);

	memset(pdwScreenBuffer_351628, 255, 640*480);
	sub_85CC3_draw_round_frame((uint16_t *)x_DWORD_17DE38str.x_DWORD_17DE5C_border_bitmap);
	MBEXsaveBitmap(path, "gameWorldMapForeground", 640, 480, pdwScreenBuffer_351628, (TColor *)*xadatapald0dat2.colorPalette_var28, true,255);
	MBEXsaveBitmap(path, "gameWorldMap", 1280, 960, x_DWORD_17DE38str.x_DWORD_17DE64_game_world_map, (TColor *)*xadatapald0dat2.colorPalette_var28, false,0);

	for (int i = 0; i < 8; i++) {
		type_E1BAC_0x3c4 *v1x = &str_E1BAC_0x2ec[i];
		sub_7AA70_load_and_decompres_dat_file(dataPath, &x_DWORD_E9C38_smalltit[307200], v1x->dword_0, v1x->dword_4);
		sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DE3C, v1x->dword_8, v1x->dword_12);
		sprintf(pathBuffer , "player%d", i);
		MBEXsaveBitmap(path, pathBuffer, v1x->word_20, v1x->word_22, &x_DWORD_E9C38_smalltit[307200], (TColor *)x_DWORD_17DE38str.x_DWORD_17DE3C, false, 0);
	}

	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 7); //4,6,7,12,14,15
	MBEXsaveBitmap(path, "multiplayer", 640, 480, x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226, (TColor *)*xadatapald0dat2.colorPalette_var28, false, 0);
	for (int i = 0; i <= 32; i++)
		MBEXsaveSprite(path + "/7", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28,true);
	for (int i = 0; i <= 271; i++)
		MBEXsaveSprite(path + "/7-B", i, xy_DWORD_17DEC0_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);
	
	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 12); //4,6,7,12,14,15
	for (int i = 0; i <= 32; i++)
		MBEXsaveSprite(path + "/12", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);
	for (int i = 0; i <= 271; i++)
		MBEXsaveSprite(path + "/12-B", i, xy_DWORD_17DEC0_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);

	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 14); //4,6,7,12,14,15
	for (int i = 0; i <= 110; i++)
		MBEXsaveSprite(path + "/14", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);
	for (int i = 0; i <= 271; i++)
		MBEXsaveSprite(path + "/14-B", i, xy_DWORD_17DEC0_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);

	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 15); //4,6,7,12,14,15
	for (int i = 0; i <= 110; i++)
		MBEXsaveSprite(path + "/15", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);
	for (int i = 0; i <= 271; i++)
		MBEXsaveSprite(path + "/15-B", i, xy_DWORD_17DEC0_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28, true);
	
	
	//char dataPath[MAX_PATH];
	sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SCREENS/HSCREEN0.DAT");
	Ref<FileAccess> file = FileAccess::open(dataPath, FileAccess::READ);
	if (file.is_null()) {
		UtilityFunctions::printerr("Failed to open file.");
		return;
	}
	file->seek(0x178E5F);
	PackedByteArray compressed_buffer = file->get_buffer(0x32B9);
	file->seek(0x17C118);
	PackedByteArray compressed_buffer2 = file->get_buffer(0x300);
	file->close();
	memcpy(x_DWORD_E9C38_smalltit, compressed_buffer.ptr(), 0x32B9);
	sub_5C3D0_file_decompress(x_DWORD_E9C38_smalltit, x_DWORD_E9C38_smalltit);
	memcpy(*xadatapald0dat2.colorPalette_var28, compressed_buffer2.ptr(), 0x300);
	MBEXsaveBitmap(path, "welcomeScreen", 320, 200, x_DWORD_E9C38_smalltit, (TColor *)*xadatapald0dat2.colorPalette_var28, false,0);
}

void MBEXsmatsConverts(String path) {
	MBEXsmatConverts(path + "", 320, 200, "SMALTIT.DAT", "SMALTIT.PAL");
	MBEXsmatConverts(path + "", 320, 200, "SMATITL2.DAT", "SMATITL2.PAL");
	MBEXsmatConverts(path + "", 320, 200, "SMATITLE.DAT", "SMATITLE.PAL");
	MBEXtextureConverts(path + "", 320, 200, "TITLE3.DAT", "PALTIT3.DAT", false);
	//MBEXtextureConvertsP(path + "", 320, 200, "TITBASF.DAT");//palette not found
}

void MBEXbullConverts(String path, String texture, String palette, int max_images) {
	std::string datPath = GetSubDirectoryFile(cdFolder.c_str(), "DATA", texture.utf8().get_data());
	std::string tabPath = datPath.substr(0, datPath.length() - 3) + "TAB";
	std::string palettePath = GetSubDirectoryFile(cdFolder.c_str(), "DATA", palette.utf8().get_data());

	uint8_t *tempTMAPS00TAB_BEGIN_BUFFER;
	type_E9C08 *temp_E9C08x;
	subtype_x_DWORD_E9C28_str *temp_F66F0x[504];

	if (!make_dir_godot(path)) {
		return;
	}

	PackedByteArray palette_data;
	Ref<FileAccess> palFile = FileAccess::open(String(palettePath.c_str()), FileAccess::READ);
	if (palFile.is_valid()) {
		palette_data = palFile->get_buffer(palFile->get_length());
		palFile->close();
	}

	std::vector<uint8_t> palette_final;
	palette_final.assign(palette_data.ptr(), palette_data.ptr() + palette_data.size());
	if (palette_final.size() < 768) {
		UtilityFunctions::print("Error: Palette is missing or too small.");
		return;
	}

	PackedByteArray fileDat_data;
	Ref<FileAccess> datFile = FileAccess::open(String(datPath.c_str()), FileAccess::READ);
	if (datFile.is_valid()) {
		fileDat_data = datFile->get_buffer(datFile->get_length());
		datFile->close();
	}

	PackedByteArray fileTab_data;
	Ref<FileAccess> tabFile = FileAccess::open(String(tabPath.c_str()), FileAccess::READ);
	if (tabFile.is_valid()) {
		fileTab_data = tabFile->get_buffer(tabFile->get_length());
		tabFile->close();
	}

	uint8_t *contentTMAPSdat = (uint8_t *)fileDat_data.ptr();
	uint8_t *contentTMAPStab = (uint8_t *)fileTab_data.ptr();

	int dword_0xE6_heapsize_230 = 0x400000;
	x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226 = (uint8_t *)Malloc_83CD0(dword_0xE6_heapsize_230);
	x_DWORD_E9C28_str = sub_71B40(dword_0xE6_heapsize_230, 0x1F8u, (type_x_DWORD_E9C28_str *)x_D41A0_BYTEARRAY_4_struct.pointer_0xE2_heapbuffer_226);
	tempTMAPS00TAB_BEGIN_BUFFER = contentTMAPStab;
	temp_E9C08x = sub_72120(0x1F8u);

	std::vector<uint8_t> buffer(256000); // Dostatečný buffer pro dekompresi
	int indextab = 0;
	int index = 0;

	while (index < max_images) {
		int shift = *(uint32_t *)&contentTMAPStab[indextab + 4];
		uint8_t *stmpdat = &contentTMAPSdat[shift];

		while ((*(uint32_t *)stmpdat) != 0x1434e52) {
			shift++;
			stmpdat = &contentTMAPSdat[shift + 1];
		}

		uint32_t unpacksize = stmpdat[7] + (stmpdat[6] << 8) + (stmpdat[5] << 16) + (stmpdat[4] << 24);

		sub_5C3D0_file_decompress(&contentTMAPSdat[shift], buffer.data());

		uint8_t *indexPtr = 10 * index + tempTMAPS00TAB_BEGIN_BUFFER;
		temp_F66F0x[index] = LoadTMapMetadata_71E70(x_DWORD_E9C28_str, (unsigned __int16)(4 * ((unsigned int)(*(x_DWORD *)indexPtr + 13) >> 2)), index);

		//type_particle_str** index6 = temp_F66F0[index];
		//uint8_t **subpointer = (uint8_t **)temp_F66F0[index];
		temp_F66F0x[index]->partstr_0 = (type_particle_str *)malloc(unpacksize);
		memcpy(temp_F66F0x[index]->partstr_0, buffer.data(), unpacksize);

		if (temp_F66F0x[index]->partstr_0->word_0 & 1)
			sub_721C0_initTmap(temp_E9C08x, &temp_F66F0x[index]->partstr_0, index);

		indextab += 10;
		index++;
	}

	type_E9C08 *tempAnimations_E9C08x;
	tempAnimations_E9C08x = sub_72120(0x1F8u);

		/*
		index = 0;
		while (index < max_images) {
			uint8_t *subpointer = *(uint8_t **)temp_F66F0x[index];
			subpointer[0] |= 8;
			index++;
		}*/

		sub_715B0();
		index = 0;

		for (index = 0; index < max_images; index++) {
			type_particle_str *subpointer = temp_F66F0x[index]->partstr_0;
			type_particle_str **index6x = &temp_F66F0x[index]->partstr_0;
			type_animations1 *anim;
			int width = subpointer->width;
			int height = subpointer->height;
			uint8_t *indices = (uint8_t *)subpointer->textureBuffer;
			int countOfFrames = 1;
			if ((*index6x)->word_0 & 1) {
				anim = sub_721C0_initTmap(tempAnimations_E9C08x, index6x, index);
				
				width = anim->Particles_4->width;
				height = anim->Particles_4->height;
				indices = (uint8_t *)anim->Particles_4->textureBuffer;
				countOfFrames = anim->CountOfFrames_16;
			}
			for (int mainindex = 0; mainindex < countOfFrames; mainindex++) {
			if (mainindex > 0) {
				sub_72350(anim);
				width = anim->Particles_4->width;
				height = anim->Particles_4->height;
				indices = (uint8_t *)anim->Particles_4->textureBuffer;
			}

			bool alpha = (indices[0] == 0 || indices[width - 1] == 0 ||
					indices[(height - 1) * width] == 0 || indices[width * height - 1] == 0);

			std::vector<unsigned char> rgba_data(width * height * 4);
			for (int i = 0; i < width * height; i++) {
				uint8_t idx = indices[i];
				int d = i * 4;
				rgba_data[d + 0] = palette_final[idx * 3 + 0] * 4;
				rgba_data[d + 1] = palette_final[idx * 3 + 1] * 4;
				rgba_data[d + 2] = palette_final[idx * 3 + 2] * 4;
				rgba_data[d + 3] = (alpha && idx == 0) ? 0 : 255;
			}

			char buf[64];
			snprintf(buf, sizeof(buf), "%s_%03i_%02i.png", texture.utf8().get_data(), index, mainindex);
			String outPath = path + "/" + String(buf);

			Ref<FileAccess> f = FileAccess::open(outPath, FileAccess::WRITE);
			if (f.is_valid()) {
				stbi_write_png_to_func(
						[](void *context, void *data, int size) {
							FileAccess *fa = static_cast<FileAccess *>(context);
							fa->store_buffer(static_cast<const uint8_t *>(data), size);
						},
						(void *)f.ptr(), width, height, 4, rgba_data.data(), width * 4);
				f->close();
			}
		}
	}
	sub_72550(&tempAnimations_E9C08x);
	UtilityFunctions::print("Extraction Completed via Godot API");
}

void MBEXtmapsCompare(String folder0, String folder1, String folder2, String outFolder) {
	if (!make_dir_godot(outFolder)) {
		return;
	}
	TypedArray<String> sourceFolders;
	sourceFolders.push_back(folder0);
	sourceFolders.push_back(folder1);
	sourceFolders.push_back(folder2);
	int max_index = 504;
	for (int i = 0; i < max_index; i++) {
		char indexBuf[8];
		snprintf(indexBuf, sizeof(indexBuf), "%03d", i);
		String strIndex = String(indexBuf);

		Vector<Ref<Image>> images;
		int maxHeight = 0;
		int totalWidth = 0;
		for (int f = 0; f < 3; f++) {
			String currentFolder = sourceFolders[f];
			String fileName = "";
			PackedStringArray files = DirAccess::get_files_at(currentFolder);
			for (int j = 0; j < files.size(); j++) {
				if (files[j].contains("_" + strIndex + "_00.png")) {
					fileName = files[j];
					break;
				}
			}
			Ref<Image> img;
			if (fileName != "") {
				img = Image::load_from_file(currentFolder + "/" + fileName);
			}
			if (img.is_null() || img->is_empty()) {
				img = Image::create(64, 64, false, Image::FORMAT_RGBA8);
				img->fill(Color(1, 1, 1, 1)); // Bílá
			}

			images.push_back(img);
			totalWidth += img->get_width();
			if (img->get_height() > maxHeight)
				maxHeight = img->get_height();
		}
		Ref<Image> combined = Image::create(totalWidth, maxHeight, false, Image::FORMAT_RGBA8);
		combined->fill(Color(0, 0, 0, 0));
		int currentX = 0;
		for (int f = 0; f < 3; f++) {
			int yOffset = (maxHeight - images[f]->get_height()) / 2;
			combined->blit_rect(images[f], Rect2i(0, 0, images[f]->get_width(), images[f]->get_height()), Vector2i(currentX, yOffset));
			currentX += images[f]->get_width();
		}
		String outPath = outFolder + "/Compare_" + strIndex + ".png";
		Error err = combined->save_png(outPath);

		if (err == OK) {
			UtilityFunctions::print("Saved comparison: ", outPath);
		} else {
			UtilityFunctions::print("Failed to save: ", outPath);
		}
	}
	UtilityFunctions::print("Comparison generation completed.");
}

void MBEXfontConverts(String path, String texture, String palette) {
	std::string datPath = GetSubDirectoryFile(cdFolder.c_str(), "DATA", texture.utf8().get_data());
	std::string tabPath = datPath.substr(0, datPath.length() - 3) + "TAB";
	std::string palettePath = GetSubDirectoryFile(cdFolder.c_str(), "DATA", palette.utf8().get_data());

	String outPath = path + "/" + texture + ".png";

	if (!make_dir_godot(path)) {
		return;
	}

	PackedByteArray palette_data;
	Ref<FileAccess> palFile = FileAccess::open(String(palettePath.c_str()), FileAccess::READ);
	if (palFile.is_valid()) {
		palette_data = palFile->get_buffer(palFile->get_length());
		palFile->close();
	}

	std::vector<uint8_t> palette_final;
	if (palette_data.size() < 768 && !palette_data.is_empty()) {
		std::vector<uint8_t> pal_dst(2048);
		int pal_dec_size = DataFileRNC::Decompress((uint8_t *)palette_data.ptr(), pal_dst.data());
		palette_final.assign(pal_dst.begin(), pal_dst.begin() + 768);
	} else {
		palette_final.assign(palette_data.ptr(), palette_data.ptr() + (palette_data.size() >= 768 ? 768 : palette_data.size()));
	}

	PackedByteArray fileDat_data;
	Ref<FileAccess> datFile = FileAccess::open(String(datPath.c_str()), FileAccess::READ);
	if (datFile.is_valid()) {
		fileDat_data = datFile->get_buffer(datFile->get_length());
		datFile->close();
	}
	std::vector<uint8_t> dat_dst(5000000);
	std::vector<uint8_t> dat_final;
	int dat_dec_size = DataFileRNC::Decompress((uint8_t *)fileDat_data.ptr(), dat_dst.data());
	dat_final.assign(dat_dst.begin(), dat_dst.begin() + dat_dec_size);
	if (dat_dec_size==0)
		dat_final.assign(fileDat_data.ptr(), fileDat_data.ptr() + fileDat_data.size());

	PackedByteArray fileTab_data;
	Ref<FileAccess> tabFile = FileAccess::open(String(tabPath.c_str()), FileAccess::READ);
	if (tabFile.is_valid()) {
		fileTab_data = tabFile->get_buffer(tabFile->get_length());
		tabFile->close();
	}
	std::vector<uint8_t> tab_dst(5000000);
	std::vector<uint8_t> tab_final;
	int tab_dec_size = DataFileRNC::Decompress((uint8_t *)fileTab_data.ptr(), tab_dst.data());
	tab_final.assign(tab_dst.begin(), tab_dst.begin() + tab_dec_size);
	if (tab_dec_size==0)
		tab_final.assign(fileTab_data.ptr(), fileTab_data.ptr() + fileTab_data.size());

	bitmap_pos_struct_tm *contentTMAPStab = (bitmap_pos_struct_tm *)tab_final.data();
	uint8_t *contentTMAPSdat = (uint8_t *)dat_final.data();

	int count = fileTab_data.size() / sizeof(bitmap_pos_struct_t);

	bool success = true;
	for (int index = 0; index < count; index++) {
		int begin = contentTMAPStab[index].data;
		int end = contentTMAPStab[index + 1].data;
		uint8_t *stmpdat = &contentTMAPSdat[begin];
		int width = contentTMAPStab[index].width_4;
		int height = contentTMAPStab[index].height_5;
		int size = end - begin;

		bitmap_pos_struct_t a3;
		a3.data = (uint8 *)contentTMAPStab[index].data;
		a3.width_4 = contentTMAPStab[index].width_4;
		a3.height_5 = contentTMAPStab[index].height_5;
		//GameBitmapDrawTransparentBitmap_2DE80(0, 0, a3, 0);//20ee80
		memset(pdwScreenBuffer_351628, 0, 640 * 480);
		GameBitmap::DrawMenuGraphic(contentTMAPStab[index].width_4, contentTMAPStab[index].height_5, 1, stmpdat, pdwScreenBuffer_351628);

		//--------------------------
		int p_channels = 4;
		std::vector<unsigned char> rgba_data((size_t)width * height * p_channels);
		const uint8_t *indices = pdwScreenBuffer_351628;

		bool alpha = true;
		bool alpha2 = (indices[0] == 0 ||
				indices[width - 1] == 0 ||
				indices[(height - 1) * width] == 0 ||
				indices[width * height - 1] == 0);
		if (!alpha2)
			alpha = false;
		for (int i = 0; i < width * height; i++) {
			uint8_t index = indices[i];
			int pal_idx = index * 3;
			int dest_idx = i * 4;

			rgba_data[dest_idx + 0] = palette_final[pal_idx + 0] * 4; // Red
			rgba_data[dest_idx + 1] = palette_final[pal_idx + 1] * 4; // Green
			rgba_data[dest_idx + 2] = palette_final[pal_idx + 2] * 4; // Blue

			rgba_data[dest_idx + 3] = 255;
			if (alpha) {
				if (index == 0) {
					rgba_data[dest_idx + 3] = 0; // transparent
				}
			}

			rgba_data[dest_idx + 0] = pal_idx; // Red
			rgba_data[dest_idx + 1] = pal_idx; // Green
			rgba_data[dest_idx + 2] = pal_idx; // Blue

		}
		char buf[16];
		snprintf(buf, sizeof(buf), "%03d", (int)index);
		String outPath = path + "/" + texture + "_" + String(buf) + ".png";
		// Otevření souboru přes Godot API
		Ref<FileAccess> f = FileAccess::open(outPath, FileAccess::WRITE);
		if (f.is_valid()) {
			// Volání STB s Lambda funkcí přímo v argumentu
			stbi_write_png_to_func(
					[](void *context, void *data, int size) {
						// Context je náš FileAccess*
						FileAccess *fa = static_cast<FileAccess *>(context);
						fa->store_buffer(static_cast<const uint8_t *>(data), size);
					},
					(void *)f.ptr(), // Předání ukazatele na FileAccess objekt
					width,
					height,
					p_channels,
					rgba_data.data(),
					width * p_channels);

			f->flush();
			f->close();
			UtilityFunctions::print("PNG saved (lambda): ", outPath);
		}
	}
}

void MBEXfontsConverts(String path) {
	MBEXfontConverts(path + "/FONT0", "FONT0.DAT", "PALD-0.DAT");
	MBEXfontConverts(path + "/FONT1", "FONT1.DAT", "PALD-0.DAT");
	MBEXfontConverts(path + "/FONT2", "FONT2.DAT", "PALD-0.DAT");
	MBEXfontConverts(path + "/DFONT0", "DFONT0.DAT", "PALD-0.DAT");
	MBEXfontConverts(path + "/DFONT1", "DFONT1.DAT", "PALD-0.DAT");
	MBEXfontConverts(path + "/HFONT3", "HFONT3.DAT", "PALD-0.DAT");
	MBEXfontConverts(path + "/SFONT1", "SFONT1.DAT", "PALD-0.DAT");
}

void MBEXtmapsConverts(String path) {
	MBEXbullConverts(path + "/TMAPS-day", "TMAPS0-0.DAT", "PALD-0.DAT",504);
	MBEXbullConverts(path + "/TMAPS-night", "TMAPS1-0.DAT", "PALN-0.DAT",504);
	MBEXbullConverts(path + "/TMAPS-cave", "TMAPS2-0.DAT", "PALC-0.DAT",464);
	MBEXtmapsCompare(path + "/TMAPS-day", path + "/TMAPS-night", path + "/TMAPS-cave", path + "/TMAPS-compare");
}

void MBEXwebConverts(String path) {
	MBEXgraphicConverts(path + "/web-day", "HWEBD0-0.DAT", "PALD-0.DAT");
	MBEXgraphicConverts(path + "/web-night", "HWEBN0-0.DAT", "PALN-0.DAT");
	MBEXgraphicConverts(path + "/web-cave", "HWEBC0-0.DAT", "PALC-0.DAT");
}

void MBEXgraphicsConverts(String path) {
	MBEXgraphicConverts(path + "/HSPR-day", "HSPRD0-0.DAT", "PALD-0.DAT");
	MBEXgraphicConverts(path + "/HSPR-night", "HSPRN0-0.DAT", "PALN-0.DAT");
	MBEXgraphicConverts(path + "/HSPR-cave", "HSPRC0-0.DAT", "PALC-0.DAT");	
}
void MBEXtexturesConverts(String path) {
	MBEXtextureConverts(path + "/day", 256, 608, "BLOCK32.DAT", "PALD-0.DAT", true);
	MBEXtextureConverts(path + "/night", 256, 608, "BL32N0-0.DAT", "PALN-0.DAT", true);
	MBEXtextureConverts(path + "/cave", 256, 608, "BL32C0-0.DAT", "PALC-0.DAT", true);
	MBEXtextureConverts(path + "/final", 256, 608, "BL32F0-0.DAT", "PALF-0.DAT", true);
}

void MBEXtextureConverts(String path, int inWidth, int inHeight, String texture, String palette, bool makeBorders) {
	std::string textPath = GetSubDirectoryFile(cdFolder.c_str(), "DATA", texture.utf8().get_data());
	std::string palettePath = GetSubDirectoryFile(cdFolder.c_str(), "DATA", palette.utf8().get_data());

	String outPath = path + "/" + texture + ".png";
	String outPath2 = path + "/" + texture + "-borders.png";

	if (!make_dir_godot(path)) {
		return;
	}

	Ref<FileAccess> texFile = FileAccess::open(String(textPath.c_str()), FileAccess::READ);
	if (texFile.is_null())
		return;
	PackedByteArray rawPixels = texFile->get_buffer(texFile->get_length());
	texFile->close();

	PackedByteArray palette_data;
	Ref<FileAccess> palFile = FileAccess::open(String(palettePath.c_str()), FileAccess::READ);
	if (palFile.is_valid()) {
		palette_data = palFile->get_buffer(palFile->get_length());
		palFile->close();
	}

	std::vector<uint8_t> palette_final;
	if (palette_data.size() < 768 && !palette_data.is_empty()) {
		std::vector<uint8_t> pal_dst(2048);
		int pal_dec_size = DataFileRNC::Decompress((uint8_t *)palette_data.ptr(), pal_dst.data());
		palette_final.assign(pal_dst.begin(), pal_dst.begin() + 768);
	} else {
		palette_final.assign(palette_data.ptr(), palette_data.ptr() + (palette_data.size() >= 768 ? 768 : palette_data.size()));
	}

	int channels = 4;
	std::vector<unsigned char> rgba_main((size_t)inWidth * inHeight * channels);
	for (int i = 0; i < inWidth * inHeight; ++i) {
		uint8_t index = (i < rawPixels.size()) ? rawPixels[i] : 0;
		int pal_idx = index * 3;
		rgba_main[i * 4 + 0] = palette_final[pal_idx + 0] * 4; // R
		rgba_main[i * 4 + 1] = palette_final[pal_idx + 1] * 4; // G
		rgba_main[i * 4 + 2] = palette_final[pal_idx + 2] * 4; // B
		rgba_main[i * 4 + 3] = 255; // Alfa
	}

	auto save_to_png = [](String p_path, int w, int h, int ch, const unsigned char *data) {
		Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::WRITE);
		if (f.is_valid()) {
			stbi_write_png_to_func(
					[](void *context, void *img_data, int size) {
						FileAccess *fa = static_cast<FileAccess *>(context);
						fa->store_buffer(static_cast<const uint8_t *>(img_data), size);
					},
					(void *)f.ptr(), w, h, ch, data, w * ch);
			f->flush();
			f->close();
			UtilityFunctions::print("PNG uloženo: ", p_path);
		}
	};

	save_to_png(outPath, inWidth, inHeight, channels, rgba_main.data());

	if (makeBorders) {
		const int TILE_SIZE = 32;
		const int PADDING = 8;
		const int NEW_TILE_SIZE = TILE_SIZE + (2 * PADDING);

		int tiles_x = inWidth / TILE_SIZE;
		int tiles_y = inHeight / TILE_SIZE;
		int out_width = tiles_x * NEW_TILE_SIZE;
		int out_height = tiles_y * NEW_TILE_SIZE;

		std::vector<unsigned char> out_img((size_t)out_width * out_height * channels, 0);

		for (int ty = 0; ty < tiles_y; ++ty) {
			for (int tx = 0; tx < tiles_x; ++tx) {
				for (int py = 0; py < NEW_TILE_SIZE; ++py) {
					for (int px = 0; px < NEW_TILE_SIZE; ++px) {
						int src_px = std::max(0, std::min(TILE_SIZE - 1, px - PADDING));
						int src_py = std::max(0, std::min(TILE_SIZE - 1, py - PADDING));
						int src_x = tx * TILE_SIZE + src_px;
						int src_y = ty * TILE_SIZE + src_py;
						int dst_x = tx * NEW_TILE_SIZE + px;
						int dst_y = ty * NEW_TILE_SIZE + py;
						for (int c = 0; c < channels; ++c) {
							out_img[(dst_y * out_width + dst_x) * channels + c] =
									rgba_main[(src_y * inWidth + src_x) * channels + c];
						}
					}
				}
			}
		}
		save_to_png(outPath2, out_width, out_height, channels, out_img.data());
	}
}

void MBEXmusicConverts(String path,char cardType) {
	musicDriverType_180C84 = cardType;
	InitMusicBank_8EAD0(0);
	for (int i = 1; i <= m_iNumberOfTracks; i++)
	{
		uint8_t *buffer = musicHeader_E3808->str_8.track_10[i - 1].xmiData_0;
		int32_t size = musicHeader_E3808->str_8.track_10[i - 1].xmiSize_8;
		int8_t *filename_c = musicHeader_E3808->str_8.track_10[i - 1].filename_14;
		if (!buffer || size <= 0 || !filename_c) {
			break;
		}
		String filename = String::utf8((const char *)filename_c);
		String midi_filename = filename.get_basename() + "_" + cardType + ".mid";
		String xmi_filename = filename.get_basename() + "_" + cardType + ".xmi";
		String full_path_mid = path + "/" + vformat("%03d_%s", i - 1, midi_filename);
		String full_path_xmi = path + "/" + vformat("%03d_%s", i - 1, xmi_filename);
		//String full_path = path + "/" + vformat("%03d_%s", i - 1, midi_filename);
		if (!make_dir_godot(path)) {
			break;
		}

		// --- Save XMI ---
		Ref<FileAccess> file_xmi = FileAccess::open(full_path_xmi, FileAccess::WRITE);
		if (file_xmi.is_valid()) {
			file_xmi->store_buffer(buffer, size);
			Error err = file_xmi->get_error();
			if (err != OK) {
				UtilityFunctions::push_error(vformat("ERROR in write XMI: %s (Error: %d)", full_path_xmi, (int)err));
			} else {
				file_xmi->flush();
				UtilityFunctions::print(vformat("Write sucessfull XMI: %s (%d bajtů)", full_path_xmi, size));
			}
			file_xmi->close();
		} else {
			UtilityFunctions::push_error(vformat("Can not open path for XMI: %s", full_path_xmi));
		}

		// --- Save MIDI ---
		size_t midi_size = 0;
		unsigned char *midi_buffer = TranscodeXmiToMid(buffer, size, &midi_size);
		if (!midi_buffer || midi_size == 0) {
			UtilityFunctions::push_error(vformat("ERROR converting XMI to MIDI for track %d (%s)", i - 1, filename));
			break;
		}
		Ref<FileAccess> file = FileAccess::open(full_path_mid, FileAccess::WRITE);
		if (!file.is_valid()) {
			free(midi_buffer);
			break;
		}
		file->store_buffer(midi_buffer, midi_size);
		Error err = file->get_error();
		if (err != OK) {
			UtilityFunctions::push_error(vformat("CHYBA při zápisu do souboru: %s (Error: %d)", full_path_mid, (int)err));
			free(midi_buffer);
			break;
		}
		file->flush();
		file->close();
		free(midi_buffer);
		UtilityFunctions::print(vformat("Úspěšně zapsáno MIDI: %s (%d bajtů z %d bajtů XMI)", full_path_mid, midi_size, size));
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
	if (!MBLoadSound(index)) {
		UtilityFunctions::push_error(vformat("Failed to load sound index: %d", index));
		return false;
	}

	int j = 1;
	while (true) {
		uint8_t *buffer = soundIndex_E37A0->str_8.wavs_10[j].wavData_18;
		int32_t size = soundIndex_E37A0->str_8.wavs_10[j].wavSize_26;
		int8_t *filename_c = soundIndex_E37A0->str_8.wavs_10[j].filename_0;
		if (!buffer || size <= 0 || !filename_c)
			break;

		String filename = String::utf8((const char *)filename_c);
		if (!make_dir_godot(path))
			break;

		// --- STEP 1: SAVE ORIGINAL VERSION (as-is from buffer) ---
		String full_path_orig = path + "/" + vformat("%03d_%03d_%s", index, j, filename);
		Ref<FileAccess> file_orig = FileAccess::open(full_path_orig, FileAccess::WRITE);
		if (file_orig.is_valid()) {
			file_orig->store_buffer(buffer, size);
			file_orig->flush();
			UtilityFunctions::print(vformat("Step 1/2 - Original saved: %s", full_path_orig));
		}

		// --- STEP 2: EXTRACT RAW PCM FROM BUFFER ---
		// Buffer may already contain a full WAV file - parse it to find raw audio data
		uint8_t *audio_data = buffer;
		int32_t audio_size = size;

		if (size > 44 && buffer[0] == 'R' && buffer[1] == 'I' &&
				buffer[2] == 'F' && buffer[3] == 'F') {
			// Skip RIFF + WAVE identifier (12 bytes), then scan for "data" chunk
			int offset = 12;
			while (offset + 8 <= size) {
				uint32_t chunk_size = buffer[offset + 4] | (buffer[offset + 5] << 8) | (buffer[offset + 6] << 16) | (buffer[offset + 7] << 24);
				if (buffer[offset] == 'd' && buffer[offset + 1] == 'a' &&
						buffer[offset + 2] == 't' && buffer[offset + 3] == 'a') {
					audio_data = buffer + offset + 8;
					audio_size = (int32_t)chunk_size;
					UtilityFunctions::print(vformat("WAV detected: data offset=%d, audio_size=%d", offset + 8, audio_size));
					break;
				}
				// Advance to next chunk (8 byte header + chunk body)
				offset += 8 + (int)chunk_size;
			}
		}

		// Safety check - make sure we didn't go out of bounds
		if (audio_data + audio_size > buffer + size) {
			UtilityFunctions::push_error(vformat("Audio data out of bounds for index %d, entry %d", index, j));
			j++;
			continue;
		}

		// --- STEP 3: RESAMPLE TO 16-bit 44.1kHz ---
		const int FADE_SAMPLES = 64;
		int new_sample_count = audio_size * 2;
		int data_chunk_size = new_sample_count * 2; // each sample is int16 = 2 bytes

		PackedByteArray resampled_data;
		resampled_data.resize(data_chunk_size);
		int16_t *dst = reinterpret_cast<int16_t *>(resampled_data.ptrw());

		for (int i = 0; i < audio_size; i++) {
			// Convert unsigned 8-bit to signed 16-bit
			int16_t sample_current = (static_cast<int16_t>(audio_data[i]) - 128) << 8;

			// Last sample fades to silence instead of copying itself -> no click at the end
			int16_t sample_next = (i < audio_size - 1)
					? (static_cast<int16_t>(audio_data[i + 1]) - 128) << 8
					: 0;

			int16_t s0 = sample_current;
			int16_t s1 = (int16_t)(((int32_t)sample_current + (int32_t)sample_next) / 2);

			// Fade-in: prevent click at the start (jump from silence)
			if (i < FADE_SAMPLES) {
				float t = (float)i / (float)FADE_SAMPLES;
				s0 = (int16_t)((float)s0 * t);
				s1 = (int16_t)((float)s1 * t);
			}

			// Fade-out: prevent click at the end
			if (i >= audio_size - FADE_SAMPLES) {
				float t = (float)(audio_size - 1 - i) / (float)FADE_SAMPLES;
				s0 = (int16_t)((float)s0 * t);
				s1 = (int16_t)((float)s1 * t);
			}

			dst[i * 2] = s0;
			dst[i * 2 + 1] = s1;
		}

		// --- STEP 4: WRITE WAV HEADER + RESAMPLED DATA ---
		String resampled_path = path + "/" + vformat("%03d_%03d_%s_441.WAV", index, j, filename.get_basename());
		Ref<FileAccess> file_res = FileAccess::open(resampled_path, FileAccess::WRITE);
		if (file_res.is_valid()) {
			// RIFF chunk
			file_res->store_8('R');
			file_res->store_8('I');
			file_res->store_8('F');
			file_res->store_8('F');
			file_res->store_32(36 + data_chunk_size); // total size minus 8 bytes of RIFF header

			// WAVE identifier
			file_res->store_8('W');
			file_res->store_8('A');
			file_res->store_8('V');
			file_res->store_8('E');

			// fmt chunk
			file_res->store_8('f');
			file_res->store_8('m');
			file_res->store_8('t');
			file_res->store_8(' ');
			file_res->store_32(16); // fmt chunk size
			file_res->store_16(1); // PCM format
			file_res->store_16(1); // Mono
			file_res->store_32(44100); // sample rate
			file_res->store_32(44100 * 2); // byte rate = SampleRate * NumChannels * BitsPerSample/8
			file_res->store_16(2); // block align = NumChannels * BitsPerSample/8
			file_res->store_16(16); // bits per sample

			// data chunk
			file_res->store_8('d');
			file_res->store_8('a');
			file_res->store_8('t');
			file_res->store_8('a');
			file_res->store_32(data_chunk_size);
			file_res->store_buffer(resampled_data);
			file_res->flush();
			UtilityFunctions::print(vformat("Step 2/2 - Resampled saved: %s", resampled_path));
		}

		j++;
	}

	return (j > 1);
}
bool MBLoadSound(uint8_t soundIndex) //265300
{
	FILE *file;
	long sizeOfFile;
	int16_t lastSoundBank[6];
	int32_t lastSoundBankPos;
	uint8_t soundIndex2 = 0;

	std::string soundPath = GetSubDirectoryFile(cdFolder.c_str(), "SOUND", "SOUND.DAT");
	file = DataFileIO::CreateOrOpenFile(soundPath.c_str(), 512);

	if (file != NULL) {
		DataFileIO::Seek(file, 0, 2);
		sizeOfFile = myftell(file);
		DataFileIO::Seek(file, sizeOfFile - 4, 0);
		DataFileIO::Read(file, (uint8_t *)&lastSoundBankPos, 4);
		DataFileIO::Seek(file, lastSoundBankPos, 0);
		DataFileIO::Read(file, (uint8_t *)lastSoundBank, 12);
		SoundNumber_E37B6 = 822;
		switch (SoundNumber_E37B6) {
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

std::ifstream g_image_file;

const uint32_t BIN_SECTOR_SIZE = 2352;
const uint32_t BIN_DATA_OFFSET = 16;
const uint32_t ISO_DATA_SIZE = 2048;

const uint32_t G_BASE_START = 0;

bool read_sector_callback(l9660_fs *fs, void *buf, uint32_t sector) {
	if (!g_image_file.is_open())
		return false;
	std::streamoff target_pos = (std::streamoff)G_BASE_START +
			((std::streamoff)sector * BIN_SECTOR_SIZE) +
			BIN_DATA_OFFSET;

	g_image_file.seekg(target_pos, std::ios::beg);
	g_image_file.read(static_cast<char *>(buf), ISO_DATA_SIZE);

	return g_image_file.good();
}

void save_file_to_disk(l9660_file *l_file, const fs::path &dest_path) {
	std::ofstream out(dest_path, std::ios::binary);
	if (!out)
		return;

	char buffer[ISO_DATA_SIZE];
	size_t read_bytes;

	while (l_file->position < l_file->length) {
		if (l9660_read(l_file, buffer, sizeof(buffer), &read_bytes) == L9660_OK) {
			if (read_bytes == 0)
				break;
			out.write(buffer, read_bytes);
		} else {
			break;
		}
	}
}

void extract_recursive(l9660_fs *fs_ptr, l9660_dir *current_dir, const fs::path &current_local_path) {
	l9660_dirent *dent;
	l9660_status status;

	l9660_seekdir(current_dir, 0);

	while (true) {
		status = l9660_readdir(current_dir, &dent);
		if (status != L9660_OK || dent == nullptr)
			break;

		std::string name(dent->name, dent->name_len);
		size_t semi_pos = name.find(';');
		if (semi_pos != std::string::npos)
			name = name.substr(0, semi_pos);

		if (name.length() == 1 && (name[0] == '\0' || name[0] == '\1'))
			continue;

		fs::path target_path = current_local_path / name;

		if (dent->flags & (1 << 1)) { // DENT_ISDIR
			std::cout << "Directory: " << target_path.string() << std::endl;
			fs::create_directories(target_path);

			l9660_dir sub_dir;
			if (l9660_opendirat(&sub_dir, current_dir, name.c_str()) == L9660_OK) {
				extract_recursive(fs_ptr, &sub_dir, target_path);
			}
		} else {
			std::cout << "File:  " << name << std::endl;
			l9660_file l_file;
			if (l9660_openat(&l_file, current_dir, name.c_str()) == L9660_OK) {
				save_file_to_disk(&l_file, target_path);
			}
		}
	}
}

// Globální Godot FileAccess místo std::ifstream
static Ref<FileAccess> g_godot_image_file;

bool read_sector_callback_godot(l9660_fs *fs, void *buf, uint32_t sector) {
	if (g_godot_image_file.is_null())
		return false;
	uint64_t target_pos = (uint64_t)G_BASE_START +
			((uint64_t)sector * BIN_SECTOR_SIZE) + BIN_DATA_OFFSET;
	g_godot_image_file->seek(target_pos);
	PackedByteArray data = g_godot_image_file->get_buffer(ISO_DATA_SIZE);
	if (data.size() < (int)ISO_DATA_SIZE)
		return false;
	memcpy(buf, data.ptr(), ISO_DATA_SIZE);
	return true;
}

void save_file_godot(l9660_file *l_file, String dest_path) {
	Ref<FileAccess> out = FileAccess::open(dest_path, FileAccess::WRITE);
	if (out.is_null())
		return;
	char buffer[ISO_DATA_SIZE];
	size_t read_bytes;
	while (l_file->position < l_file->length) {
		if (l9660_read(l_file, buffer, sizeof(buffer), &read_bytes) == L9660_OK) {
			if (read_bytes == 0)
				break;
			out->store_buffer((const uint8_t *)buffer, read_bytes);
		} else
			break;
	}
}

void extract_recursive_godot(l9660_fs *fs_ptr, l9660_dir *current_dir, String current_local_path) {
	l9660_dirent *dent;
	l9660_seekdir(current_dir, 0);

	while (true) {
		if (l9660_readdir(current_dir, &dent) != L9660_OK || dent == nullptr)
			break;

		std::string name(dent->name, dent->name_len);
		size_t semi = name.find(';');
		if (semi != std::string::npos)
			name = name.substr(0, semi);
		if (name.length() == 1 && (name[0] == '\0' || name[0] == '\1'))
			continue;

		String godot_name = String(name.c_str());
		String target_path = current_local_path.path_join(godot_name);

		if (dent->flags & (1 << 1)) {
			DirAccess::make_dir_recursive_absolute(target_path);
			l9660_dir sub_dir;
			if (l9660_opendirat(&sub_dir, current_dir, name.c_str()) == L9660_OK)
				extract_recursive_godot(fs_ptr, &sub_dir, target_path);
		} else {
			l9660_file l_file;
			if (l9660_openat(&l_file, current_dir, name.c_str()) == L9660_OK)
				save_file_godot(&l_file, target_path);
		}
	}
}

// Return codes:
//  2  = OK, direct copy (branch B)
//  1  = OK, ISO extraction (branch A)
// -1  = GAME folder not found
// -2  = no files found in src (empty directory?)
// -3  = l9660_openfs failed
// -4  = NETHERW.EXE not found (branch B)
// -5  = failed to create output_path
// -6  = copyDir(src, output_path) failed (src unreadable?)
// -7  = failed to create gameBase subdirectories
// -8  = no required CDATA file was copied
// -9  = no required CLEVELS file was copied

int MBEXcdExtract(char *pathGOG, char *pathOut) {
	String src = String(pathGOG);
	String dst = String(pathOut);

	auto copyFile = [&](String from, String to) -> int {
		if (!FileAccess::file_exists(from))
			return -1;
		Ref<FileAccess> fin = FileAccess::open(from, FileAccess::READ);
		Ref<FileAccess> fout = FileAccess::open(to, FileAccess::WRITE);
		if (fin.is_null() || fout.is_null())
			return -2;
		const int CHUNK = 1024 * 1024;
		uint64_t remaining = fin->get_length();
		while (remaining > 0) {
			uint64_t toRead = MIN((uint64_t)CHUNK, remaining);
			fout->store_buffer(fin->get_buffer(toRead));
			remaining -= toRead;
		}
		return 1;
	};

	std::function<bool(String, String)> copyDir = [&](String from, String to) -> bool {
		DirAccess::make_dir_recursive_absolute(to);
		Ref<DirAccess> dir = DirAccess::open(from);
		if (dir.is_null())
			return false;
		dir->list_dir_begin();
		String name = dir->get_next();
		while (name != "") {
			if (name != "." && name != "..") {
				String fromFull = from.path_join(name);
				String toFull = to.path_join(name);
				if (dir->current_is_dir())
					copyDir(fromFull, toFull);
				else
					copyFile(fromFull, toFull);
			}
			name = dir->get_next();
		}
		dir->list_dir_end();
		return true;
	};

	auto safeCopy = [&](String from, String to) -> bool {
		if (FileAccess::file_exists(from) && !FileAccess::file_exists(to))
			return copyFile(from, to) > 0;
		return false;
	};

	// --- Copy GAME folder ---
	String gameFolder = src.path_join("GAME");
	if (DirAccess::dir_exists_absolute(gameFolder)) {
		copyDir(gameFolder, dst.path_join("GAME"));
	} else {
		UtilityFunctions::printerr("[-1] GAME folder not found in: " + src);
		return -1;
	}

	// --- Find largest file (ISO/BIN) ---
	String biggestFilePath = "";
	uint64_t maxSize = 0;

	std::function<void(String)> findBiggest = [&](String folder) {
		Ref<DirAccess> dir = DirAccess::open(folder);
		if (dir.is_null())
			return;
		dir->list_dir_begin();
		String name = dir->get_next();
		while (name != "") {
			if (name != "." && name != "..") {
				String full = folder.path_join(name);
				if (dir->current_is_dir()) {
					findBiggest(full);
				} else {
					Ref<FileAccess> f = FileAccess::open(full, FileAccess::READ);
					if (f.is_valid()) {
						uint64_t size = f->get_length();
						if (size > maxSize) {
							maxSize = size;
							biggestFilePath = full;
						}
					}
				}
			}
			name = dir->get_next();
		}
		dir->list_dir_end();
	};

	findBiggest(src);

	if (biggestFilePath == "") {
		UtilityFunctions::printerr("[-2] No files found in src: " + src);
		return -2;
	}

	// --- Branch A: ISO image via l9660 ---
	g_godot_image_file = FileAccess::open(biggestFilePath, FileAccess::READ);
	if (g_godot_image_file.is_valid()) {
		l9660_fs fs_ctx;
		l9660_dir root_dir;
		if (l9660_openfs(&fs_ctx, read_sector_callback_godot) == L9660_OK) {
			l9660_fs_open_root(&root_dir, &fs_ctx);
			String output_path = dst.path_join("CD_Files");
			if (DirAccess::make_dir_recursive_absolute(output_path) != OK) {
				g_godot_image_file.unref();
				UtilityFunctions::printerr("[-5] Failed to create output dir: " + output_path);
				return -5;
			}
			extract_recursive_godot(&fs_ctx, &root_dir, output_path);
			g_godot_image_file.unref();
			UtilityFunctions::print("[1] Done! ISO extraction successful.");
			return 1;
		} else {
			g_godot_image_file.unref();
			UtilityFunctions::printerr("[-3] l9660_openfs failed on: " + biggestFilePath);
			return -3;
		}
	}

	// --- Branch B: direct file copy (no ISO) ---
	if (!FileAccess::file_exists(src.path_join("NETHERW.EXE"))) {
		UtilityFunctions::printerr("[-4] NETHERW.EXE not found in: " + src);
		return -4;
	}

	String output_path = dst.path_join("CD_Files");
	if (DirAccess::make_dir_recursive_absolute(output_path) != OK) {
		UtilityFunctions::printerr("[-5] Failed to create output dir: " + output_path);
		return -5;
	}

	if (!copyDir(src, output_path)) {
		UtilityFunctions::printerr("[-6] Failed to copy src dir: " + src);
		return -6;
	}

	String gameBase = dst.path_join("GAME/NETHERW");
	bool dirsOK =
			DirAccess::make_dir_recursive_absolute(gameBase.path_join("CDATA")) == OK &&
			DirAccess::make_dir_recursive_absolute(gameBase.path_join("CLEVELS")) == OK &&
			DirAccess::make_dir_recursive_absolute(gameBase.path_join("LANGUAGE")) == OK &&
			DirAccess::make_dir_recursive_absolute(gameBase.path_join("SAVE")) == OK &&
			DirAccess::make_dir_recursive_absolute(gameBase.path_join("SHOTS")) == OK &&
			DirAccess::make_dir_recursive_absolute(gameBase.path_join("SOUND")) == OK;
	if (!dirsOK) {
		UtilityFunctions::printerr("[-7] Failed to create one or more gameBase subdirectories under: " + gameBase);
		return -7;
	}

	// --- Copy CDATA files ---
	int cdataCopied = 0;
	cdataCopied += safeCopy(src.path_join("DATA/TMAPS0-0.DAT"), gameBase.path_join("CDATA/TMAPS0-0.DAT")) ? 1 : 0;
	cdataCopied += safeCopy(src.path_join("DATA/TMAPS1-0.DAT"), gameBase.path_join("CDATA/TMAPS1-0.DAT")) ? 1 : 0;
	cdataCopied += safeCopy(src.path_join("DATA/TMAPS2-0.DAT"), gameBase.path_join("CDATA/TMAPS2-0.DAT")) ? 1 : 0;
	cdataCopied += safeCopy(src.path_join("DATA/VERSION.DAT"), gameBase.path_join("CDATA/VERSION.DAT")) ? 1 : 0;
	cdataCopied += safeCopy(src.path_join("DATA/TMAPS0-0.TAB"), gameBase.path_join("CDATA/TMAPS0-0.TAB")) ? 1 : 0;
	cdataCopied += safeCopy(src.path_join("DATA/TMAPS1-0.TAB"), gameBase.path_join("CDATA/TMAPS1-0.TAB")) ? 1 : 0;
	cdataCopied += safeCopy(src.path_join("DATA/TMAPS2-0.TAB"), gameBase.path_join("CDATA/TMAPS2-0.TAB")) ? 1 : 0;
	if (cdataCopied == 0) {
		UtilityFunctions::printerr("[-8] No CDATA files were copied — DATA/ folder missing or empty in: " + src);
		return -8;
	}
	UtilityFunctions::print("[B] CDATA files copied: " + String::num_int64(cdataCopied) + "/7");

	// --- Copy CLEVELS files ---
	int clevelsCopied = 0;
	clevelsCopied += safeCopy(src.path_join("LEVELS/LEVELS.DAT"), gameBase.path_join("CLEVELS/LEVELS.DAT")) ? 1 : 0;
	clevelsCopied += safeCopy(src.path_join("LEVELS/LEVELS.TAB"), gameBase.path_join("CLEVELS/LEVELS.TAB")) ? 1 : 0;
	if (clevelsCopied == 0) {
		UtilityFunctions::printerr("[-9] No CLEVELS files were copied — LEVELS/ folder missing or empty in: " + src);
		return -9;
	}
	UtilityFunctions::print("[B] CLEVELS files copied: " + String::num_int64(clevelsCopied) + "/2");

	// --- Copy SOUND files (optional, skip SOUND.DAT and MUSIC.DAT) ---
	String soundSrc = src.path_join("SOUND");
	String soundDst = gameBase.path_join("SOUND");
	if (DirAccess::dir_exists_absolute(soundSrc)) {
		Ref<DirAccess> sdir = DirAccess::open(soundSrc);
		if (sdir.is_valid()) {
			int soundCopied = 0;
			sdir->list_dir_begin();
			String fname = sdir->get_next();
			while (fname != "") {
				if (fname != "." && fname != ".." && !sdir->current_is_dir()) {
					if (fname != "SOUND.DAT" && fname != "MUSIC.DAT") {
						if (safeCopy(soundSrc.path_join(fname), soundDst.path_join(fname)))
							soundCopied++;
					}
				}
				fname = sdir->get_next();
			}
			sdir->list_dir_end();
			UtilityFunctions::print("[B] SOUND files copied: " + String::num_int64(soundCopied));
		} else {
			UtilityFunctions::printerr("[B] Warning: SOUND folder exists but could not be opened: " + soundSrc);
		}
	} else {
		UtilityFunctions::printerr("[B] Warning: SOUND folder not found (non-fatal): " + soundSrc);
	}

	UtilityFunctions::print("[2] Done! Direct copy successful.");
	return 2;
}



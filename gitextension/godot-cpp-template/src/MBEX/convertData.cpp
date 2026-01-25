#include "convertData.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
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

#if defined(_WIN32)
#include <direct.h>
#define MKDIR(path) _wmkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

using namespace godot;

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

void MBEXconvertData(String path) {
	MBEXsoundConverts(path + "/sounds");
	MBEXmusicConverts(path + "/musics");
	MBEXtexturesConverts(path + "/textures");
	MBEXgraphicsConverts(path + "/HSPR");
	MBEXwebConverts(path + "/web");
	MBEXsmatsConverts(path + "/smat");
	MBEXhscreenConverts(path + "/HSCREEN");
	MBEXhtablesConverts(path + "/TABLES");
	//MBEXhbuttonsConverts(path + "/BUTTONS");-only noise
	MBEXcdrConverts(path + "/CDR");
	MBEXpointersConverts(path + "/POINTERS");//-find true palette
	MBEXgtdConverts(path + "/GTD");
	MBEXextractLang(path + "/language", "res://hidata/language/", "/LANGUAGE/");
}

void MBEXextractLang(String path, String langPath, String cdLangPath) {
	if (!make_dir_godot(path)) {
		return;
	}
	String cdFullLangPath = String(cdFolder) + cdLangPath;
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
		UtilityFunctions::printerr("MBEX: Nepodařilo se otevřít zdrojovou složku: ", langPath);
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
					UtilityFunctions::printerr("MBEX: Chyba při kopírování ", file_name);
				}
			}
		}
		file_name = dir->get_next();
	}
	dir->list_dir_end();

	char dataPath[MAX_PATH];
	sub_7AA70_load_and_decompres_dat_file(dataPath, (uint8_t *)x_DWORD_17DE38str.x_DWORD_17DE38x, 0xB2947, 768);

	dir = DirAccess::open(langPath);
	if (dir.is_valid()) {
		dir->list_dir_begin();
		String f_name = dir->get_next();
		while (f_name != "") {
			if (f_name != "." && f_name != "..") {
				if (f_name.begins_with("l")) {
					String full_path = langPath.path_join(f_name);
					CharString full_pathUtf = full_path.utf8();
					Ref<FileAccess> file = FileAccess::open(full_path, FileAccess::READ);
					if (file.is_valid()) {
						uint64_t file_len = file->get_length();
						PackedByteArray data = file->get_buffer(file_len);
						memcpy(x_DWORD_E9C38_smalltit, data.ptr(), file_len);
						MBEXsaveBitmapCrop(path, (char *)f_name.utf8().get_data(), 90, 53, x_DWORD_E9C38_smalltit + 1, (TColor *)x_DWORD_17DE38str.x_DWORD_17DE38x, 2);
						UtilityFunctions::print("Soubor nacten a zpracovan: ", f_name);
					} else {
						UtilityFunctions::printerr("Nepodarilo se otevrit soubor: ", full_path);
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
	std::string textPath = GetSubDirectoryFile(cdFolder, "DATA", texture.utf8().get_data());
	std::string palettePath = GetSubDirectoryFile(cdFolder, "DATA", palette.utf8().get_data());
	String outPath = path + "/" + texture + ".bmp";
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
		UtilityFunctions::print("Chyba: Paletu se nepodarilo dekomprimovat nebo je poskozena.");
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
		UtilityFunctions::print("Chyba: Data neobsahuji dostatek pixelu pro rozmery: ", inWidth, "x", inHeight);
		return;
	}
	int p_channels = 3;
	std::vector<unsigned char> rgb_data((size_t)inWidth * inHeight * p_channels);
	const uint8_t *indices = img_final.data() + data_start_offset;
	for (int i = 0; i < inWidth * inHeight; ++i) {
		uint8_t index = indices[i];
		int pal_idx = index * 3;

		rgb_data[i * 3 + 0] = palette_final[pal_idx + 2]*4; // Blue
		rgb_data[i * 3 + 1] = palette_final[pal_idx + 1]*4; // Green
		rgb_data[i * 3 + 2] = palette_final[pal_idx + 0]*4; // Red
	}
	save_bmp_godot(outPath, inWidth, inHeight, p_channels, rgb_data.data());
	UtilityFunctions::print("Ulozeno: ", outPath, " (", inWidth, "x", inHeight, ")");
}

#pragma pack(1)
typedef struct {
	uint32_t data;
	uint8_t width_4;
	uint8_t height_5;
} bitmap_pos_struct_tm;
#pragma pack(pop)

void MBEXgraphicConverts(String path, String texture, String palette) {
	std::string datPath = GetSubDirectoryFile(cdFolder, "DATA", texture.utf8().get_data());
	std::string tabPath = datPath.substr(0, datPath.length() - 3) + "TAB";
	std::string palettePath = GetSubDirectoryFile(cdFolder, "DATA", palette.utf8().get_data());	

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
		UtilityFunctions::print("Chyba: Paletu se nepodarilo dekomprimovat nebo je poskozena.");
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

	int count = fileTab_data.size() / sizeof(bitmap_pos_struct_t);

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
		memset(pdwScreenBuffer_351628, 0, 100000);
		GameBitmap::DrawMenuGraphic(contentTMAPStab[index].width_4, contentTMAPStab[index].height_5, 1, stmpdat, pdwScreenBuffer_351628);

		//--------------------------
		int p_channels = 4;
		std::vector<unsigned char> rgba_data((size_t)width * height * p_channels);
		const uint8_t *indices = pdwScreenBuffer_351628;

		for (int i = 0; i < width * height; ++i) {
			uint8_t index = indices[i];
			int pal_idx = index * 3;
			int dest_idx = i * 4;

			// Základní barvy (pozor na pořadí, PNG chce standardně RGB)
			rgba_data[dest_idx + 0] = palette_final[pal_idx + 0] * 4; // Red
			rgba_data[dest_idx + 1] = palette_final[pal_idx + 1] * 4; // Green
			rgba_data[dest_idx + 2] = palette_final[pal_idx + 2] * 4; // Blue

			// 2. Logika průhlednosti: pokud je index 0, alfa = 0 (průhledná), jinak 255 (neprůhledná)
			if (index == 0) {
				rgba_data[dest_idx + 3] = 0; // Průhledná
			} else {
				rgba_data[dest_idx + 3] = 255; // Neprůhledná
			}
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
			UtilityFunctions::print("PNG ulozeno (lambda): ", outPath);
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

void MBEXsaveSprite(String path, int i, bitmap_pos_struct_t bitmap, TColor* palette) {
	int inWidth = bitmap.width_4;
	int inHeight = bitmap.height_5;
	uint8_t* data = bitmap.data;
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
	memset(pdwScreenBuffer_351628, 0, 100000);
	GameBitmap::DrawMenuGraphic(inWidth, inHeight, 1, data, pdwScreenBuffer_351628);

	//--------------------------
	int p_channels = 4;
	std::vector<unsigned char> rgba_data((size_t)inWidth * inHeight * p_channels);
	const uint8_t *indices = pdwScreenBuffer_351628;

	for (int i = 0; i < inWidth * inHeight; ++i) {
		uint8_t index = indices[i];
		int pal_idx = index * 3;
		int dest_idx = i * 4;

		// Základní barvy (pozor na pořadí, PNG chce standardně RGB)
		rgba_data[dest_idx + 0] = palette_final[pal_idx + 0] * 4; // Red
		rgba_data[dest_idx + 1] = palette_final[pal_idx + 1] * 4; // Green
		rgba_data[dest_idx + 2] = palette_final[pal_idx + 2] * 4; // Blue

		// 2. Logika průhlednosti: pokud je index 0, alfa = 0 (průhledná), jinak 255 (neprůhledná)
		if (index == 0) {
			rgba_data[dest_idx + 3] = 0; // Průhledná
		} else {
			rgba_data[dest_idx + 3] = 255; // Neprůhledná
		}
	}

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
	//for (int i = 0; i < inWidth * inHeight; ++i)
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

void MBEXsaveBitmap(String path, char *name, int width, int height, uint8_t *data, TColor *palette, bool alpha) {
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
	for (int i = 0; i < inWidth * inHeight; ++i) {
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

void MBEXhscreenConverts(String path) {
	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 4);//4,6,7,12,14,15
	for (int i = 0; i <= 312; i++)
		MBEXsaveSprite(path + "/4", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28);

	MBEXsaveBitmap(path, "manuBackground", 640, 480, x_DWORD_E9C38_smalltit, (TColor *)*xadatapald0dat2.colorPalette_var28, false);

	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 6); //4,6,7,12,14,15
	for (int i = 0; i <= 312; i++)
		MBEXsaveSprite(path + "/6", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28);

	MBEXsaveBitmap(path, "gameWorldMap", 1280, 960, x_DWORD_17DE38str.x_DWORD_17DE64_game_world_map, (TColor *)*xadatapald0dat2.colorPalette_var28, false);

	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 7); //4,6,7,12,14,15
	for (int i = 0; i <= 32; i++)
		MBEXsaveSprite(path + "/7", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28);
	/*
	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 12); //4,6,7,12,14,15
	for (int i = 0; i <= 312; i++)
		MBEXsaveSprite(path + "/12", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28);

	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 14); //4,6,7,12,14,15
	for (int i = 0; i <= 312; i++)
		MBEXsaveSprite(path + "/14", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28);

	sub_7A110_load_hscreen(x_WORD_180660_VGA_type_resolution, 15); //4,6,7,12,14,15
	for (int i = 0; i <= 312; i++)
		MBEXsaveSprite(path + "/15", i, xy_DWORD_17DED4_spritestr[i], (TColor *)*xadatapald0dat2.colorPalette_var28);
	*/
	
	char dataPath[MAX_PATH];
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
	MBEXsaveBitmap(path, "welcomeScreen", 320, 200, x_DWORD_E9C38_smalltit, (TColor *)*xadatapald0dat2.colorPalette_var28, false);
}

void MBEXsmatsConverts(String path) {
	MBEXsmatConverts(path + "", 320, 200, "SMALTIT.DAT", "SMALTIT.PAL");
	MBEXsmatConverts(path + "", 320, 200, "SMATITL2.DAT", "SMATITL2.PAL");
	MBEXsmatConverts(path + "", 320, 200, "SMATITLE.DAT", "SMATITLE.PAL");
	MBEXtextureConverts(path + "", 320, 200, "TITLE3.DAT", "PALTIT3.DAT", false);
	//MBEXtextureConvertsP(path + "", 320, 200, "TITBASF.DAT");//palette not found
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
	std::string textPath = GetSubDirectoryFile(cdFolder, "DATA", texture.utf8().get_data());
	std::string palettePath = GetSubDirectoryFile(cdFolder, "DATA", palette.utf8().get_data());
	String outPath = path + "/" + texture + ".bmp";
	String outPath2 = path + "/" + texture + "-borders.bmp";
	std::string stdOutPath = outPath.utf8().get_data();

	if (!make_dir_godot(path)) {
		return;
	}

// Načtení dat textury pomocí Godot FileAccess
	Ref<FileAccess> texFile = FileAccess::open(String(textPath.c_str()), FileAccess::READ);
	if (texFile.is_null())
		return;
	PackedByteArray rawPixels = texFile->get_buffer(texFile->get_length());


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
		UtilityFunctions::print("Chyba: Paletu se nepodarilo dekomprimovat nebo je poskozena.");
		return;
	}

	// Otevření souboru pro zápis (Godot si s res:// poradí sám)
	Ref<FileAccess> bFile = FileAccess::open(outPath, FileAccess::WRITE);
	if (bFile.is_null())
		return;

	// --- VÝPOČTY ---
	int paddingSize = (4 - (inWidth % 4)) % 4;
	int rowSize = inWidth + paddingSize;
	int pixelDataSize = rowSize * inHeight;
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
	bFile->store_32(inWidth);
	bFile->store_32(inHeight);
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
		if (i * 3 + 2 < palette_final.size()) {
			bFile->store_8(palette_final[i * 3 + 2] * 4); // B
			bFile->store_8(palette_final[i * 3 + 1] * 4); // G
			bFile->store_8(palette_final[i * 3] * 4); // R
			bFile->store_8(0); // Reserved
		} else {
			bFile->store_32(0); // Vyplnění prázdné palety
		}
	}

	// --- PIXEL DATA (Bottom-up) ---
	for (int y = inHeight - 1; y >= 0; y--) {
		int rowStart = y * inWidth;
		if (rowStart < rawPixels.size()) {
			// Vezmeme pod-pole pro aktuální řádek a uložíme ho
			PackedByteArray row = rawPixels.slice(rowStart, rowStart + inWidth);
			bFile->store_buffer(row);
		} else {
			// Padding pokud data chybí
			for (int x = 0; x < inWidth; x++)
				bFile->store_8(0);
		}

		// Zápis paddingu na konci řádku
		for (int p = 0; p < paddingSize; p++) {
			bFile->store_8(0);
		}
	}

	bFile->flush();
	bFile->close();

	if (makeBorders) {
		//const char *input_path = "input.bmp";
		//const char *output_path = "output.bmp";
		const int TILE_SIZE = 32;
		const int PADDING = 8;
		const int NEW_TILE_SIZE = TILE_SIZE + (2 * PADDING);

		BMPData input = load_bmp_godot(outPath);

		if (input.pixels.empty()) {
			std::cerr << "Chyba: Nepodarilo se nacist BMP ze souboru: " << outPath.utf8().get_data() << std::endl;
			return;
		}

		int width = input.width;
		int height = input.height;
		int channels = input.channels;
		unsigned char *img = input.pixels.data(); // Ukazatel na surová data uvnitř vektoru

		int tiles_x = width / TILE_SIZE;
		int tiles_y = height / TILE_SIZE;
		int out_width = tiles_x * NEW_TILE_SIZE;
		int out_height = tiles_y * NEW_TILE_SIZE;

		std::vector<unsigned char> out_img(out_width * out_height * channels, 0);

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
									img[(src_y * width + src_x) * channels + c];
						}
					}
				}
			}
		}

		save_bmp_godot(outPath2, out_width, out_height, channels, out_img.data());
	}
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

#define _CRT_SECURE_NO_WARNINGS
// DeRncMC2Files.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
/*
PackedByteArray deRNC(PackedByteArray bytearray) {
	int input_size = bytearray.size();
	std::vector<uint8_t> src(input_size);
	for (int i = 0; i < input_size; ++i)
		src[i] = bytearray[i];
	int output_size = input_size * 20;
	std::vector<uint8_t> dst(output_size);
	int decompressed_size = DataFileRNC::Decompress(src.data(), dst.data());
	PackedByteArray result;
	result.resize(decompressed_size);
	for (int i = 0; i < decompressed_size; ++i)
		result[i] = dst[i];
	return result;
}
*/
bool file_exists(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    return f.good();
}
void SaveBuildings(const char* path)
{
    char dat_path[512];
    char tab_path[512];
    sprintf(dat_path, "%s.DAT", path);
    sprintf(tab_path, "%s.TAB", path);

    // otevøení TAB souboru
    std::ifstream tab_file(tab_path, std::ios::binary);
    if (!tab_file) {
        return;
    }
    // naètení TAB souboru (int32 offsety)
    std::vector<int32_t> tab_file_arr;

    while (true) {
        int32_t value;
        tab_file.read(reinterpret_cast<char*>(&value), sizeof(int32_t));
        if (!tab_file) {
            break;
        }
        tab_file_arr.push_back(value);
    }

    tab_file.close();

    int index = 0;
    while (true)
    {

        // výpoèet offsetu a velikosti
        int32_t start = tab_file_arr[index];
        int32_t end = tab_file_arr[index + 1];
        int32_t size = end - start;
        size = 7 * 7 * 7 * 7;
        // otevøení DAT souboru
        std::ifstream dat_file(dat_path, std::ios::binary);
        if (!dat_path) {
            return;
        }

        // seek na zaèátek levelu
        dat_file.seekg(start, std::ios::beg);

        // naètení komprimovaných dat
        std::vector<uint8_t> level_tab_data(size);
        dat_file.read(
            reinterpret_cast<char*>(level_tab_data.data()),
            size
        );

        dat_file.close();
        index++;
    }
}

int main()
{
	SaveBuildings("c:/prenos/godot-zyllan/MagicBalls/gitextension/godot-cpp-template/data/CD_Files/DATA/BUILD0-0");
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

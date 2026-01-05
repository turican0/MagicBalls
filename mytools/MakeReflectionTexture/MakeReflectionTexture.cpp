#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master\stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master\stb_image_write.h"

#include <iostream>
#include <vector>
#include <set>

// Definice struktury pro jeden pixel
struct Pixel {
    unsigned char r, g, b, a;

    // Operátor pro porovnávání v std::set (aby set věděl, co je duplicita)
    bool operator<(const Pixel& other) const {
        if (r != other.r) return r < other.r;
        if (g != other.g) return g < other.g;
        if (b != other.b) return b < other.b;
        return a < other.a;
    }
};

int main() {
    const char* inputPath = "c:\\prenos\\godot-zyllan\\MagicBalls\\test-terrain\\levels\\tmaps\\out-vert.png";
    const char* outputPath = "c:\\prenos\\godot-zyllan\\MagicBalls\\test-terrain\\levels\\tmaps\\out-vert-refl.png";

    int width, height, channels;
    // Načtení obrázku (vynutíme 4 kanály - RGBA)
    unsigned char* img = stbi_load(inputPath, &width, &height, &channels, 4);

    if (!img) {
        std::cerr << "Chyba: Obrazek nebyl nalezen!" << std::endl;
        return 1;
    }

    // --- 1. Sběr unikátních pixelů z oblasti ---
    // Definuj oblast (x, y, šířka, výška)
    int areaX = 0, areaY = 0, areaW = 32, areaH = 32;
    std::set<Pixel> uniquePixels;

    for (int y = areaY; y < areaY + areaH && y < height; ++y) {
        for (int x = areaX; x < areaX + areaW && x < width; ++x) {
            int index = (y * width + x) * 4;
            Pixel p = { img[index], img[index + 1], img[index + 2], img[index + 3] };
            uniquePixels.insert(p);
        }
    }

    std::cout << "Nasbirano unikatnich pixelu: " << uniquePixels.size() << std::endl;

    // --- 2. Vytvoření nového obrázku ---
    std::vector<unsigned char> outputImg(width * height * 4, 0);

    for (int ytile = 0; ytile < height/32; ++ytile)
    for (int xtile = 0; xtile < width/32; ++xtile)
    for (int y = 0; y < 32; ++y)
    for (int x = 0; x < 32; ++x) {
        int idx = (ytile*32*32*8+xtile*32+y*32*8+x) * 4;
        Pixel current = { img[idx], img[idx + 1], img[idx + 2], img[idx + 3] };
		bool dis = false;
		/*if (xtile == 0 && ytile == 0)
            dis = false;
        if (xtile == 0 && ytile == 18)
            dis = false;
        if (xtile == 18 && ytile == 18)
            dis = false;
        if (xtile == 0 && ytile == 19)
            dis = false;
        if (xtile == 1 && ytile == 19)
            dis = false;
        if (xtile == 0 && ytile == 22)
            dis = false;
        if (xtile == 1 && ytile == 22)
            dis = false;
        if (xtile == 0 && ytile == 23)
            dis = false;
        if (xtile == 1 && ytile == 23)
            dis = false;
        if (xtile == 0 && ytile == 40)
            dis = false;
        if (xtile == 1 && ytile == 40)
            dis = false;
        if (xtile == 0 && ytile == 41)
            dis = false;
        if (xtile == 1 && ytile == 41)
            dis = false;
        if (xtile == 0 && ytile == 51)
            dis = false;
        if (xtile == 1 && ytile == 51)
            dis = false;
        if (xtile == 0 && ytile == 52)
            dis = false;
        if (xtile == 1 && ytile == 52)
            dis = false;

        if (xtile == 0 && ytile == 54)
            dis = false;
        if (xtile == 1 && ytile == 54)
            dis = false;
        if (xtile == 0 && ytile == 55)
            dis = false;
        if (xtile == 1 && ytile == 55)
            dis = false;
        if (xtile == 0 && ytile == 57)
            dis = false;
        if (xtile == 1 && ytile == 57)
            dis = false;
        if (xtile == 0 && ytile == 58)
            dis = false;
        if (xtile == 1 && ytile == 58)
            dis = false;
        if (xtile == 0 && ytile == 59)
            dis = false;
        if (xtile == 1 && ytile == 59)
            dis = false;
        if (xtile == 0 && ytile == 63)
            dis = false;
        if (xtile == 1 && ytile == 63)
            dis = false;*/

        // Pokud pixel existuje v našem seznamu unikátních barev
        if ((uniquePixels.find(current) != uniquePixels.end())&&(!dis)) {
            outputImg[idx] = 255;     // Cerveny kanal
            outputImg[idx + 1] = 0;   // Zeleny
            outputImg[idx + 2] = 0;   // Modry
            outputImg[idx + 3] = 255; // Alfa (neprůhledné)
        }
        else {
            outputImg[idx] = 0;
            outputImg[idx + 1] = 0;
            outputImg[idx + 2] = 0;
            outputImg[idx + 3] = 255; // Cerny podklad
        }
    }

    // --- 3. Uložení ---
    stbi_write_png(outputPath, width, height, 4, outputImg.data(), width * 4);

    std::cout << "Hotovo! Obrazek ulozen jako: " << outputPath << std::endl;

    stbi_image_free(img);
    return 0;
}
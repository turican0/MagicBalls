#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "../MakeReflectionTexture/stb-master/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../MakeReflectionTexture/stb-master/stb_image_write.h"

#include <iostream>
#include <vector>
#include <string>

int main() {
    const char* input_path = "c:/prenos/godot-zyllan/MagicBalls/test-terrain/levels/tmaps/out-vert-refl.png";
    const char* output_path = "c:/prenos/godot-zyllan/MagicBalls/test-terrain/levels/tmaps/out-vert-refl-border.png";
    const int TILE_SIZE = 32;
    const int PADDING = 8;
    const int NEW_TILE_SIZE = TILE_SIZE + (2 * PADDING);

    int width, height, channels;
    unsigned char* img = stbi_load(input_path, &width, &height, &channels, 0);

    if (!img) {
        std::cerr << "Chyba: Nepodarilo se nacist soubor " << input_path << std::endl;
        return 1;
    }

    int tiles_x = width / TILE_SIZE;
    int tiles_y = height / TILE_SIZE;
    int out_width = tiles_x * NEW_TILE_SIZE;
    int out_height = tiles_y * NEW_TILE_SIZE;

    std::vector<unsigned char> out_img(out_width * out_height * channels, 0);

    for (int ty = 0; ty < tiles_y; ++ty) {
        for (int tx = 0; tx < tiles_x; ++tx) {

            // Prochazime kazdy pixel v nove "padded" dlazdici (48x48)
            for (int py = 0; py < NEW_TILE_SIZE; ++py) {
                for (int px = 0; px < NEW_TILE_SIZE; ++px) {

                    // Vypocet souradnic ve zdrojove dlazdici (0-31) se zastropovanim (clamp)
                    int src_px = px - PADDING;
                    int src_py = py - PADDING;

                    if (src_px < 0) src_px = 0;
                    if (src_px >= TILE_SIZE) src_px = TILE_SIZE - 1;
                    if (src_py < 0) src_py = 0;
                    if (src_py >= TILE_SIZE) src_py = TILE_SIZE - 1;

                    // Pozice v puvodnim a novem poli
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

    stbi_write_png(output_path, out_width, out_height, channels, out_img.data(), out_width * channels);

    std::cout << "Hotovo! Ulozeno do: " << output_path << std::endl;
    stbi_image_free(img);
    return 0;
}
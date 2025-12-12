#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#define IMAGE_FILENAME "output.png"

typedef double real_t;

// Unused, but since I have already written this correctly first try,
// I'm going to leave this as a testament of my skill, or luck.
static bool write_ppm(const char* filename, const int width, const int height, const real_t* image) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open %s\n", filename);
        return false;
    }

    fprintf(file, "P3\n%d %d\n255\n", width, height);
    for (int y = 0, i = 0; y < height; y++) {
        for (int x = 0; x < width; x++, i += 3) {
            const int r = (int)((real_t)255.999 * image[i]);
            const int g = (int)((real_t)255.999 * image[i + 1]);
            const int b = (int)((real_t)255.999 * image[i + 2]);

            if (x > 0) {
                fprintf(file, " ");
            }
            fprintf(file, "%d %d %d", r, g, b);
        }
        fprintf(file, "\n");
    }

    return true;
}

int main(const int argc, const char** argv) {
    (void)argc;
    (void)argv;

    static const real_t x_step = 1.0 / (IMAGE_WIDTH - 1);
    static const real_t y_step = 1.0 / (IMAGE_HEIGHT - 1);

    uint8_t* image = malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 3 * sizeof(*image));

    int i = 0;
    for (double y = 0; y < 1.0; y += y_step) {
        for (double x = 0; x < 1.0; x += x_step, i += 3) {
            image[i] = (uint8_t)(x * 255.999);
            image[i + 1] = (uint8_t)(y * 255.999);
            image[i + 2] = 0;
        }
    }

    stbi_write_png(IMAGE_FILENAME, IMAGE_WIDTH, IMAGE_HEIGHT, 3, image, IMAGE_WIDTH * 3 * sizeof(*image));

    free(image);
    return EXIT_SUCCESS;
}

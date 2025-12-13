#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define CVKM_LH
#include <cvkm.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define IMAGE_WIDTH 400
#define ASPECT_RATIO ((real)(16.0 / 9.0))
#define IMAGE_HEIGHT ((int)(IMAGE_WIDTH / ASPECT_RATIO))
#define FOCAL_LENGTH ((real)1)
#define VIEWPORT_HEIGHT ((real)2)
#define VIEWPORT_WIDTH (VIEWPORT_HEIGHT * ((real)IMAGE_WIDTH / IMAGE_HEIGHT))
#define CAMERA_CENTER CVKM_DVEC3_ZERO
#define PIXEL_DELTA_U (VIEWPORT_WIDTH / IMAGE_WIDTH)
#define PIXEL_DELTA_V (-VIEWPORT_HEIGHT / IMAGE_HEIGHT)
#define VIEWPORT_UPPER_LEFT ((point3){ { \
    CAMERA_CENTER.x - VIEWPORT_WIDTH / (real)2, \
    CAMERA_CENTER.y + VIEWPORT_HEIGHT / (real)2, \
    CAMERA_CENTER.z + FOCAL_LENGTH, \
} })
#define PIXEL00_LOCATION ((point3){ { \
    VIEWPORT_UPPER_LEFT.x + (real)0.5 * PIXEL_DELTA_U, \
    VIEWPORT_UPPER_LEFT.y + (real)0.5 * PIXEL_DELTA_V, \
    VIEWPORT_UPPER_LEFT.z, \
} })

#define IMAGE_FILENAME "output.png"

typedef double real;
typedef vkm_dvec3 vec3;
typedef vec3 point3;
typedef vec3 color;

typedef struct ray {
    point3 origin;
    vec3 direction;
} ray;

static_assert(IMAGE_HEIGHT >= 1, "Make the image wider!");

static void ray_at(const ray* ray, const real t, point3* result) {
    vkm_mul(&ray->direction, t, result);
    vkm_add(&ray->origin, result, result);
}

bool hit_sphere(const point3* center, const double radius, const ray* r) {
    vec3 oc;
    vkm_sub(center, &r->origin, &oc);
    const real a = vkm_dot(&r->direction, &r->direction);
    const real b = -2.0 * vkm_dot(&r->direction, &oc);
    const real c = vkm_dot(&oc, &oc) - radius * radius;
    const real discriminant = b * b - 4 * a * c;
    return discriminant >= 0;
}

static void ray_color(const ray* r, color* c) {
    if (hit_sphere(&CVKM_DVEC3_FRONT, (real)0.5, r)) {
        *c = CVKM_DVEC3_RIGHT;
        return;
    }

    vkm_dvec3 normalized;
    vkm_normalize(&r->direction, &normalized);

    const real a = (real)0.5 * (normalized.y + (real)1.0);

    color from = { { (real)1.0, (real)1.0, (real)1.0 } };
    vkm_mul(&from, (real)1.0 - a, &from);
    color to = { { (real)0.5, (real)0.7, (real)1.0 } };
    vkm_mul(&to, a, &to);

    vkm_add(&from, &to, c);
}

// Unused, but since I have already written this correctly first try,
// I'm going to leave this as a testament of my skill, or luck.
static bool write_ppm(const char* filename, const int width, const int height, const real* image) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open %s\n", filename);
        return false;
    }

    fprintf(file, "P3\n%d %d\n255\n", width, height);
    for (int y = 0, i = 0; y < height; y++) {
        for (int x = 0; x < width; x++, i += 3) {
            const int r = (int)((real)255.999 * image[i]);
            const int g = (int)((real)255.999 * image[i + 1]);
            const int b = (int)((real)255.999 * image[i + 2]);

            if (x > 0) {
                fprintf(file, " ");
            }
            fprintf(file, "%d %d %d", r, g, b);
        }
        fprintf(file, "\n");
    }

    return true;
}

static void write_color(uint8_t* image, const color* color) {
    image[0] = (uint8_t)(color->r * 255.999);
    image[1] = (uint8_t)(color->g * 255.999);
    image[2] = (uint8_t)(color->b * 255.999);
}

int main(const int argc, const char** argv) {
    (void)argc;
    (void)argv;

    uint8_t* image = malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 3 * sizeof(*image));

    int i = 0;
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        printf("Scanlines remaining: %d\n", IMAGE_HEIGHT - y);
        for (int x = 0; x < IMAGE_WIDTH; x++, i += 3) {
            point3 pixel_center = PIXEL00_LOCATION;
            pixel_center.x += x * PIXEL_DELTA_U;
            pixel_center.y += y * PIXEL_DELTA_V;

            ray ray;
            ray.origin = CAMERA_CENTER;
            vkm_sub(&pixel_center, &ray.origin, &ray.direction);

            color color;
            ray_color(&ray, &color);
            write_color(&image[i], &color);
        }
    }

    printf("Writing png...");
    if (!stbi_write_png(IMAGE_FILENAME, IMAGE_WIDTH, IMAGE_HEIGHT, 3, image, IMAGE_WIDTH * 3 * sizeof(*image))) {
        printf(" Failed to write png.\n");
    }
    printf(" Done.\n");

    free(image);
    return EXIT_SUCCESS;
}

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

void convertToGrayscale(unsigned char *input, unsigned char *output, int width, int height, int channels) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * channels;
            unsigned char r = input[index];
            unsigned char g = input[index + 1];
            unsigned char b = input[index + 2];
            // Convert to grayscale using the luminance formula
            unsigned char gray = (unsigned char)(0.3f * r + 0.59f * g + 0.11f * b);
            output[y * width + x] = gray;
        }
    }
}

void applySobelFilter(unsigned char *input, unsigned char *output, int width, int height) {
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int Gy[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            int sumX = 0;
            int sumY = 0;

            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int pixel = input[(y + ky) * width + (x + kx)];
                    sumX += pixel * Gx[ky + 1][kx + 1];
                    sumY += pixel * Gy[ky + 1][kx + 1];
                }
            }

            int magnitude = (int)sqrt(sumX * sumX + sumY * sumY);
            output[y * width + x] = (unsigned char)fminf(magnitude, 255);
        }
    }
}

int main() {
    const char *inputFilename = "steve.png";
    const char *outputFilename = "output_sobel.png";

    int width, height, channels;
    unsigned char *image = stbi_load(inputFilename, &width, &height, &channels, 0);
    if (!image) {
        fprintf(stderr, "Error loading image\n");
        return EXIT_FAILURE;
    }

    unsigned char *grayscaleImage = (unsigned char *)malloc(width * height);
    if (!grayscaleImage) {
        fprintf(stderr, "Error allocating memory for grayscale image\n");
        stbi_image_free(image);
        return EXIT_FAILURE;
    }

    convertToGrayscale(image, grayscaleImage, width, height, channels);

    unsigned char *sobelImage = (unsigned char *)malloc(width * height);
    if (!sobelImage) {
        fprintf(stderr, "Error allocating memory for Sobel image\n");
        free(grayscaleImage);
        stbi_image_free(image);
        return EXIT_FAILURE;
    }

    applySobelFilter(grayscaleImage, sobelImage, width, height);

    if (!stbi_write_png(outputFilename, width, height, 1, sobelImage, width)) {
        fprintf(stderr, "Error saving image\n");
        free(sobelImage);
        free(grayscaleImage);
        stbi_image_free(image);
        return EXIT_FAILURE;
    }

    printf("Image processed and saved to %s\n", outputFilename);

    free(sobelImage);
    free(grayscaleImage);
    stbi_image_free(image);

    return EXIT_SUCCESS;
}

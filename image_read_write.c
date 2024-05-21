#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
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

int main() {
    const char *inputFilename = "steve.png";
    const char *outputFilename = "output_grayscale.png";

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

    if (!stbi_write_png(outputFilename, width, height, 1, grayscaleImage, width)) {
        fprintf(stderr, "Error saving image\n");
        free(grayscaleImage);
        stbi_image_free(image);
        return EXIT_FAILURE;
    }

    printf("Image processed and saved to %s\n", outputFilename);

    free(grayscaleImage);
    stbi_image_free(image);

    return EXIT_SUCCESS;
}

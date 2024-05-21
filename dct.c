#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

#define BLOCK_SIZE 8

void computeDCT(const float *input, float *output, int size) {
    const float PI = 3.14159265358979323846;
    float c_u, c_v;

    for (int u = 0; u < size; ++u) {
        for (int v = 0; v < size; ++v) {
            float sum = 0.0f;
            for (int x = 0; x < size; ++x) {
                for (int y = 0; y < size; ++y) {
                    float cos1 = cosf((2 * x + 1) * u * PI / (2.0f * size));
                    float cos2 = cosf((2 * y + 1) * v * PI / (2.0f * size));
                    sum += input[x * size + y] * cos1 * cos2;
                }
            }
            c_u = (u == 0) ? 1.0f / sqrtf(2.0f) : 1.0f;
            c_v = (v == 0) ? 1.0f / sqrtf(2.0f) : 1.0f;
            output[u * size + v] = 0.25f * c_u * c_v * sum;
        }
    }
}

void blockDCT(unsigned char *input, unsigned char *output, int width, int height, int channels) {
    float block[BLOCK_SIZE * BLOCK_SIZE];
    float dctBlock[BLOCK_SIZE * BLOCK_SIZE];

    for (int y = 0; y < height; y += BLOCK_SIZE) {
        for (int x = 0; x < width; x += BLOCK_SIZE) {
            for (int c = 0; c < channels; ++c) {
                // Load block
                for (int i = 0; i < BLOCK_SIZE; ++i) {
                    for (int j = 0; j < BLOCK_SIZE; ++j) {
                        int xi = x + i;
                        int yj = y + j;
                        if (xi < width && yj < height) {
                            block[i * BLOCK_SIZE + j] = input[(yj * width + xi) * channels + c];
                        } else {
                            block[i * BLOCK_SIZE + j] = 0;
                        }
                    }
                }
                // Compute DCT
                computeDCT(block, dctBlock, BLOCK_SIZE);
                // Store DCT block back to output
                for (int i = 0; i < BLOCK_SIZE; ++i) {
                    for (int j = 0; j < BLOCK_SIZE; ++j) {
                        int xi = x + i;
                        int yj = y + j;
                        if (xi < width && yj < height) {
                            output[(yj * width + xi) * channels + c] = (unsigned char)fminf(fmaxf(dctBlock[i * BLOCK_SIZE + j], 0.0f), 255.0f);
                        }
                    }
                }
            }
        }
    }
}

int main() {
    const char *inputFilename = "steve.png";
    const char *outputFilename = "output_dct.png";

    int width, height, channels;
    unsigned char *image = stbi_load(inputFilename, &width, &height, &channels, 0);
    if (!image) {
        fprintf(stderr, "Error loading image\n");
        return EXIT_FAILURE;
    }

    unsigned char *outputImage = (unsigned char *)malloc(width * height * channels);
    if (!outputImage) {
        fprintf(stderr, "Error allocating memory for output image\n");
        stbi_image_free(image);
        return EXIT_FAILURE;
    }

    blockDCT(image, outputImage, width, height, channels);

    if (!stbi_write_png(outputFilename, width, height, channels, outputImage, width * channels)) {
        fprintf(stderr, "Error saving image\n");
        free(outputImage);
        stbi_image_free(image);
        return EXIT_FAILURE;
    }

    printf("Image processed and saved to %s\n", outputFilename);

    free(outputImage);
    stbi_image_free(image);

    return EXIT_SUCCESS;
}

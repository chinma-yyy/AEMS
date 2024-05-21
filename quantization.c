#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

#define BLOCK_SIZE 8

// Standard JPEG quantization matrix for luminance
const int quantMatrix[BLOCK_SIZE][BLOCK_SIZE] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}
};

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

void quantizeBlock(float *block, int size, const int quantMatrix[size][size]) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            block[i * size + j] = roundf(block[i * size + j] / quantMatrix[i][j]);
        }
    }
}

void blockDCTAndQuantize(unsigned char *input, unsigned char *output, int width, int height, int channels) {
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
                // Quantize DCT block
                quantizeBlock(dctBlock, BLOCK_SIZE, quantMatrix);
                // Store quantized block back to output
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
    const char *outputFilename = "output_quantized.png";

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

    blockDCTAndQuantize(image, outputImage, width, height, channels);

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

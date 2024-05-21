#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

void applyConvolution(unsigned char *input, unsigned char *output, int width, int height, int channels, float *kernel, int kernelSize) {
    int kHalf = kernelSize / 2;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                float sum = 0.0f;
                for (int ky = -kHalf; ky <= kHalf; ++ky) {
                    for (int kx = -kHalf; kx <= kHalf; ++kx) {
                        int ix = x + kx;
                        int iy = y + ky;
                        if (ix >= 0 && ix < width && iy >= 0 && iy < height) {
                            int imageIdx = (iy * width + ix) * channels + c;
                            int kernelIdx = (ky + kHalf) * kernelSize + (kx + kHalf);
                            sum += input[imageIdx] * kernel[kernelIdx];
                        }
                    }
                }
                int outputIdx = (y * width + x) * channels + c;
                output[outputIdx] = (unsigned char)fminf(fmaxf(sum, 0.0f), 255.0f);
            }
        }
    }
}

int main() {
    const char *inputFilename = "steve.png";
    const char *outputFilename = "output.png";

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

    // Example kernel: simple 3x3 sharpening kernel
    float kernel[9] = {
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    };

    applyConvolution(image, outputImage, width, height, channels, kernel, 3);

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

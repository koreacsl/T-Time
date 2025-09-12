#include <stdio.h>
#include <stdlib.h>
#include <webp/decode.h>
#include <time.h> // For measuring execution time

#include "pmparser.h"
#include "parse_pagemap.h"
#include "introspection.h"
#include <sys/mman.h>

// Function to read a file into memory
uint8_t* read_file(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *data = (uint8_t *)malloc(*size);
    if (!data) {
        fprintf(stderr, "Failed to allocate memory for file data\n");
        fclose(file);
        return NULL;
    }

    fread(data, 1, *size, file);
    fclose(file);
    return data;
}

// Function to decode WebP image
int decode_webp(const char *webp_file, const char *output_file) {
    size_t webp_size;
    uint8_t *webp_data = read_file(webp_file, &webp_size);
    if (!webp_data) {
        return -1;
    }

    int width, height;
    uint8_t *raw_data = WebPDecodeRGBA(webp_data, webp_size, &width, &height);
    free(webp_data);
          
    if (!raw_data) {
        fprintf(stderr, "Failed to decode WebP file: %s\n", webp_file);
        return -1; 
    }        

    printf("Decoded WebP file: %s (Width: %d, Height: %d)\n", webp_file, width, height);
    
    // Save raw pixel data to a binary file (optional, for visualization or debugging)
    FILE *output_fp = fopen(output_file, "wb");
    if (!output_fp) {
        fprintf(stderr, "Failed to open output file: %s\n", output_file);
        free(raw_data);
        return -1;
    }

    fwrite(raw_data, width * height * 4, 1, output_fp); // RGBA has 4 bytes per pixel
    fclose(output_fp);

    printf("Raw pixel data saved to: %s\n", output_file);
    free(raw_data);
    return 0;
}

int main(int argc, char *argv[]) {
	int ret;
	int k;
	int dummy;
	uint64_t start, end;

	const char *webp_file = "64x64.webp";
	const char *output_file = "test.txt";

	decode_webp(webp_file, output_file);

	//	ptedit_init();
	uint64_t ida[CodeLocationLength] = {
		0x000000008000,	// VP8ParseIntraModeRow
		0x00000000b000,	// VP8DecodeMB
		0x00000000a000, // GetCoeffsFast
	};

	printf("threshold done!\n");
	return 0;

}


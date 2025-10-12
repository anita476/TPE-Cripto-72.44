#ifndef _BMP_H_
#define _BMP_H_

#include <stdint.h>

// ensure correct alignment (1 byte) no padding (to process binary files)
#pragma pack(push,1)
// BMP file header
struct bmpfile_header {
    uint16_t bfType; //must be "BM" (0x4D42)
    uint32_t filesz;
    uint16_t creator1;
    uint16_t creator2;
    uint32_t bmp_offset;
};

// BMP info header
struct bmp_dib_v3_header_t {
    uint32_t header_sz;
    int32_t width;
    int32_t height; // negative for top-down, positive for down - top
    uint16_t nplanes;
    uint16_t bitspp; // bits per pixel! should be 24
    uint32_t compress_type; // 0 for no compression
    uint32_t bmp_bytesz;
    int32_t hres;
    int32_t vres;
    uint32_t ncolors;
    uint32_t nimpcolors;
};

// structure of a pixel
struct bmp_pixel_t {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

struct bmp_image_t {
    struct bmpfile_header file_header;
    struct bmp_dib_v3_header_t dib_header;
    //payload
    struct bmp_pixel_t *data;
};
// rest of program shouldnt be affected 
#pragma pack(pop)

/**
@brief Load a BMP image from a file to memory. Must be freed with cleanup function
@param filepath The path to the BMP file
@return A pointer to the BMP image or NULL if an error occurs
*/
struct bmp_image_t * load_bmp_new(const char * filepath);
void bmp_image_free(struct bmp_image_t * bmp);
#endif
#ifndef _BMP_H_
#define _BMP_H_

#include <stdint.h>

// BMP file header
struct bmpfile_header {
    uint32_t filesz;
    uint16_t creator1;
    uint16_t creator2;
    uint32_t bmp_offset;
};

// BMP info header
struct bmp_dib_v3_header_t {
    uint32_t header_sz;
    uint32_t width;
    uint32_t height;
    uint16_t nplanes;
    uint16_t bitspp;
    uint32_t compress_type;
    uint32_t bmp_bytesz;
    uint32_t hres;
    uint32_t vres;
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

#endif
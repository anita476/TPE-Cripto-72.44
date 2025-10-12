#ifndef _LSB1_H_
#define _LSB1_H_
#include <stdint.h>
#include "bmp.h"

struct bmp_image_t * stego_lsb1(struct bmp_image_t * original_bmp, uint8_t * data);

#endif
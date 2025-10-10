#ifndef _LSB1_H_
#define _LSB1_H_
#include <stdint.h>
#include "bmp.h"

bmp_image_t * stego_lsb1(bmp_image_t * original_bmp, uint_8 * data);

#endif
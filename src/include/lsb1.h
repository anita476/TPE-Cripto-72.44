#ifndef _LSB1_H_
#define _LSB1_H_
#include <stdint.h>
#include <iso646.h>
#include <stddef.h>
#include "bmp.h"

int stego_lsb1(struct bmp_image_t * original_bmp, uint8_t * message, size_t message_length);


void lsb1_extract(const struct bmp_image_t *bmp, uint8_t *out, size_t msg_len);

#endif
#ifndef _LSB4_H_
#define _LSB4_H_

#include <stdint.h>
#include <stddef.h>
#include <iso646.h>
#include "bmp.h"

/**
 * Hides a message in a BMP image using LSB4
 * 
 * @param original_bmp BMP image where the message will be hidden
 * @param message Buffer containing the message to hide
 * @param message_length Size of the message in bytes
 * @return 0 on success, 1 on error
 */
int stego_lsb4(struct bmp_image_t *original_bmp, const uint8_t *message, size_t message_length);

/**
 * Extracts a hidden message with LSB4 from a BMP image
 * 
 * @param bmp BMP image with hidden message
 * @param out Output buffer for the message
 * @param msg_len Size of the message to extract in bytes
 */
void lsb4_extract(const struct bmp_image_t *bmp, uint8_t *out, size_t msg_len);

#endif
#ifndef STEGO_UTILS_H
#define STEGO_UTILS_H

#include <stddef.h>
#include <stdint.h>

uint8_t *pack_plain(const char *in_file, const uint8_t *msg, size_t msg_len, size_t *out_len);

uint8_t *pack_encrypted(const uint8_t *cipher, size_t cipher_len, size_t *out_len);

#endif
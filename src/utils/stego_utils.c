#include <string.h>
#include <stdio.h>
#include "../include/file_utils.h"
#include "../include/stego_utils.h"

uint8_t *pack_plain(const char *in_file, const uint8_t *msg, size_t msg_len, size_t *out_len) {
    // Search for extension and calculate length
    const char *ext = strrchr(in_file, '.');
    if (!ext){
        fprintf(stderr,"Error: no file extension detected. Is the input a valid file?\n");
        return NULL;
    }
    size_t ext_len = strlen(ext) + 1;
    // Size + message + extension
    *out_len = 4 + msg_len + ext_len;
    uint8_t *buf = safe_malloc(*out_len);
    // Copying length in big endian...
    buf[0] = (msg_len >> 24) & 0xFF;
    buf[1] = (msg_len >> 16) & 0xFF;
    buf[2] = (msg_len >> 8) & 0xFF;
    buf[3] = (msg_len) & 0xFF;
    // Copying the message and the extension
    memcpy(buf + 4, msg, msg_len);
    memcpy(buf + 4 + msg_len, ext, ext_len);
    return buf;
}

uint8_t *pack_encrypted(const uint8_t *cipher, size_t cipher_len, size_t *out_len) {
    // Total size will be 4 bytes length + cipher
    *out_len = 4 + cipher_len;
    uint8_t *buf = safe_malloc(*out_len);
    // Copying length in big endian...
    buf[0] = (cipher_len >> 24) & 0xFF;
    buf[1] = (cipher_len >> 16) & 0xFF;
    buf[2] = (cipher_len >> 8) & 0xFF;
    buf[3] = (cipher_len) & 0xFF;
    // Copying the cipher
    memcpy(buf + 4, cipher, cipher_len);
    return buf;
}
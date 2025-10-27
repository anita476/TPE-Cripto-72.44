#ifndef _LSBI_H_
#define _LSBI_H_
#include <stdint.h>
#include <stdlib.h>
#include "bmp.h"

#pragma pack(push,1) // just to be safe we dont want padding added by the compiler so we can manipulate bits easily

/**
Patterns:
00
01
10
11
**/
struct byte_ref{
    uint8_t *ptr;   
    size_t index;   // pixel component number, just in case!
    struct byte_ref * next;
};

typedef struct {
    struct byte_ref *items;
    size_t count;
    size_t total;
    struct byte_ref *last;
} pattern_group;

#pragma pack(pop) 

int stego_lsbi(struct bmp_image_t * original_bmp, const uint8_t * message, size_t message_length);

void lsbi_extract(const struct bmp_image_t *bmp, uint8_t *out, size_t msg_len);

#endif
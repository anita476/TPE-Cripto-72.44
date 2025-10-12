#include "../include/lsb1.h"
#include "../include/bmp.h"
#include <stdio.h>

int stego_lsb1(struct bmp_image_t * original_bmp, const uint8_t * message, size_t message_length) {
    // first we analize capacity 


    if((message_length * 8) > original_bmp->dib_header.bmp_bytesz){
        fprintf(stderr, "Error: Message length exceeds bmp capacity\n");
        return 1;
    }
    uint8_t * data = (uint8_t *)original_bmp->data;
    size_t bit_idx = 0;

    for (size_t i = 0; i < message_length; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            uint8_t bit_value = (message[i] >> bit) & 1;
            data[bit_idx] = (data[bit_idx] & 0xFE) | bit_value; // clear LSB then set it
            bit_idx++;
        }
    }
    return 0;
}


void lsb1_extract(const struct bmp_image_t *bmp, uint8_t *out, size_t msg_len) {
    const uint8_t *data = (const uint8_t *)bmp->data;
    size_t bit_idx = 0;

    for (size_t i = 0; i < msg_len; i++) {
        uint8_t byte = 0;
        for (int bit = 7; bit >= 0; bit--) {
            byte |= (data[bit_idx] & 1) << bit;
            bit_idx++;
        }
        out[i] = byte;
    }
}

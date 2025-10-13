#include "../include/lsb4.h"
#include "../include/bmp.h"
#include <stdio.h>

int stego_lsb4(struct bmp_image_t *original_bmp, const uint8_t *message, size_t message_length)
{
    if ((message_length * 2) > original_bmp->dib_header.bmp_bytesz)
    {
        fprintf(stderr, "Error: Message length exceeds bmp capacity\n");
        return 1;
    }

    uint8_t *data = (uint8_t *)original_bmp->data;
    size_t img_idx = 0;

    for (size_t i = 0; i < message_length; i++)
    {
        uint8_t msg_byte = message[i];

        // High nibble and low nibble
        uint8_t high_nibble = (msg_byte >> 4) & 0x0F; // i feel like it's not even necessary to do this mask
        uint8_t low_nibble = msg_byte & 0x0F;
        // Embed the higher nibble (4 bits)
        data[img_idx] = (data[img_idx] & 0xF0) | high_nibble;
        img_idx++;
        // Embed the lower nibble (4 bits)
        data[img_idx] = (data[img_idx] & 0xF0) | low_nibble;
        img_idx++;
    }
    return 0;
}

void lsb4_extract(const struct bmp_image_t *bmp, uint8_t *out, size_t msg_len)
{
    const uint8_t *data = (const uint8_t *)bmp->data;
    size_t img_idx = 0;

    for (size_t i = 0; i < msg_len; i++)
    {
        uint8_t high_nibble = data[img_idx] & 0x0F;
        img_idx++;
        uint8_t low_nibble = data[img_idx] & 0x0F;
        img_idx++;

        out[i] = (high_nibble << 4) | low_nibble;
    }
}
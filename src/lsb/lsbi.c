#include "../include/lsbi.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

static int append_ref(pattern_group *group, uint8_t *ptr, size_t index);
static uint8_t get_pattern(uint8_t byte);

int stego_lsbi(struct bmp_image_t * original_bmp, const uint8_t * message, size_t message_length){
    // analysing capacity (we need 8 extra bytes: 4 for message length + 4 for pattern flags)
    if((message_length * 8 + 8) > original_bmp->dib_header.bmp_bytesz){
        fprintf(stderr, "Error: Message length exceeds bmp capacity. Max is %d\n",original_bmp->dib_header.bmp_bytesz);
        return 1;
    }
    // we first encrypt using "lsb1", whilst also keeping track of the different pattern groups
    // we must skip the first 8 bytes: 4 for message length + 4 for pattern flags
    uint8_t * data_start = (uint8_t *)original_bmp->data;
    uint8_t * lsb1_data = data_start + 8; // we advance the pointer 8 bytes
    size_t bit_idx = 0;
    // we "group" by 1st2nd bit pattern
    // possible gorups:
    // 00 -> 0
    // 01 -> 1
    // 10 -> 2
    // 11 -> 3
    pattern_group groups[4] = {0};
    for (size_t i = 0; i < message_length; i++) {
        for (int bit = 7; bit >= 0; bit--) { // for each bit in a message - byte...
            uint8_t bit_value = (message[i] >> bit) & 1;
            uint8_t original_lsb = lsb1_data[bit_idx] & 1;

            // WRONG!!! this would analyze the LSB of two consecutive bytes !!
            // uint8_t b1 = lsb1_data[bit_idx] & 1;
            // uint8_t b2 = lsb1_data[i+1] & 1;
            // uint8_t pattern = (b1 << 1) | b2; 
            
            uint8_t pattern = get_pattern(lsb1_data[bit_idx]);

            // we add to the corresponding group
            pattern_group *g = &groups[pattern];
            g->total++;

            if (bit_value != original_lsb)
                g->count++;

            // add byte reference to this group
            append_ref(g, &lsb1_data[bit_idx], bit_idx);

            lsb1_data[bit_idx] = (lsb1_data[bit_idx] & 0xFE) | bit_value; 
            bit_idx++;
        }
    }
    // now we must decide which to invert 
    for (int p = 0; p < 4; p++) {
        if (groups[p].count * 2> groups[p].total) { // if pattern p marked for inversion
            data_start[4 + p] = (data_start[4 + p] & 0xFE) | 1; // store flag in bytes 4-7
            struct byte_ref *node = groups[p].items;
            while (node) {
                *(node->ptr) ^= 1; // flip LSB in-place
                node = node->next;
            }
        }
       //else {
         //   data_start[4 + p] = data_start[4 + p] & 0xFE; // it would already be 0 but just in case...
        //}
    }
    //cleanup
    for (int p = 0; p < 4; p++) {
        struct byte_ref *cur = groups[p].items;
        while (cur) {
            struct byte_ref *next = cur->next;
            free(cur);
            cur = next;
        }
    }
    return 0;

}

void lsbi_extract(const struct bmp_image_t *bmp, uint8_t *out, size_t msg_len) {
    const uint8_t *data_start = (const uint8_t *)bmp->data;
    const uint8_t *lsb1_data = data_start + 8;
    size_t bit_idx = 0;

    uint8_t pattern_flags[4] = {0};

    for (int i = 0; i < 4; i++) {
        pattern_flags[i] = data_start[4 + i] & 1; // 1 ->pattern was inverted (read from bytes 4-7)
    }

    // extracting message bits
    for (size_t i = 0; i < msg_len; i++) {
        uint8_t current_byte = 0;

        for (int bit = 7; bit >= 0; bit--) {
            //  pattern from current cover data
            //uint8_t b1 = lsb1_data[bit_idx] & 1;
            //uint8_t b2 = lsb1_data[bit_idx + 1] & 1;
            //uint8_t pattern = (b1 << 1) | b2; // pattern = 00,01,10,11 -> 0..3

            uint8_t pattern = get_pattern(lsb1_data[bit_idx]);
            uint8_t extracted_bit = lsb1_data[bit_idx] & 1;

            // if this pattern was inverted → undo it
            if (pattern_flags[pattern]) {
                extracted_bit ^= 1;
            }

            current_byte |= (extracted_bit << bit);

            bit_idx++;
        }
        out[i] = current_byte;
    }
}

static int append_ref(pattern_group *group, uint8_t *ptr, size_t index) {
    struct byte_ref *node = malloc(sizeof(struct byte_ref));
    if (!node) {
        fprintf(stderr,"Error: Failed to allocate memory\n");
        return 1;
    }
    node->ptr = ptr;
    node->index = index;
    node->next = NULL;

    if (group->items == NULL)
        group->items = node;      // first node
    else
        group->last->next = node; // link at tail

    group->last = node;
    //group->count++;
    return 0;
}


// extract pattern 
// we remove the last bit, and then && with a "3"
// that leaves all bits in 0 except the last 2 , so we obtain all 0 but the patterns bits (2 and 3)

static uint8_t get_pattern(uint8_t byte){
    return (byte >> 1) & 0x03;
}
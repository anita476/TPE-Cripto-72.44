#include "../include/lsbi.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdbool.h>

static int append_ref(pattern_group *group, uint8_t *ptr, size_t index);
static uint8_t get_pattern(uint8_t byte);
static void lsb1_embed_no_red(uint8_t *data, const uint8_t *message, size_t message_length, size_t start_byte);
static void lsb1_extract_no_red(const uint8_t *data, uint8_t *out, size_t msg_len, size_t start_byte);

int stego_lsbi(struct bmp_image_t * original_bmp, const uint8_t * message, size_t message_length){
   
    // analyzing capacity: we need 4 bytes for pattern flags 
    // message will be embedded starting from byte 4
    // but we only use GREEN AND BLUE channels (not RED) so effective capacity is 2/3
    size_t effective_capacity = (original_bmp->dib_header.bmp_bytesz * 2) / 3;
    if ((message_length * 8 + 4 * 8) > effective_capacity) {
        fprintf(stderr, "Error: Message length exceeds bmp capacity. Max is %zu\n", effective_capacity);
        return 1;
    }

    // we first encrypt using "lsb1", whilst also keeping track of the different pattern groups
    uint8_t * data = (uint8_t *)original_bmp->data;
    size_t total_bytes = original_bmp->dib_header.bmp_bytesz;

    // step 1: store original LSBs before embedding 
    uint8_t * original_lsbs = (uint8_t *)malloc(total_bytes);
    if (!original_lsbs) {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        return 1;
    }
    for (size_t i = 0; i < total_bytes; i++) {
        original_lsbs[i] = data[i] & 1;
    }

    // apply lsb1 starting from byte 4, skipping RED channel (every 3rd byte starting from byte 2)
    lsb1_embed_no_red(data, message, message_length, 4); 

    pattern_group groups[4] = {0};
    // we "group" by 1st2nd bit pattern
    // possible gorups:
    // 00 -> 0
    // 01 -> 1
    // 10 -> 2
    // 11 -> 3
    for (size_t i = 0; i < total_bytes; i++) {
        uint8_t pattern = get_pattern(data[i]);

        groups[pattern].total++;

        if (((data[i] & 1) != original_lsbs[i]) & 1) {
            groups[pattern].count++;
        }

        // store reference to this byte
        append_ref(&groups[pattern], &data[i], i);
    }

    free(original_lsbs);

    // decide which patterns to invert
    bool invert_pattern[4] = {false};
    for (int p = 0; p < 4; p++) {
        if (groups[p].count * 2 > groups[p].total) {
            invert_pattern[p] = true;
        }
    }

    for (int p = 0; p < 4; p++) {
        if (invert_pattern[p]) {
            struct byte_ref *node = groups[p].items;
            while (node) {
                if (node->index >= 3) {  // Only invert from byte 3 onwards
                    *(node->ptr) ^= 1;
                }
                node = node->next;
            }
        }
    }
    // store the inversion flags in bytes 0-3
    for (int i = 0; i < 4; i++) {
        data[i] = (data[i] & 0xFE) | (invert_pattern[i] ? 1 : 0);
    }
    
    // cleanup
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

    const uint8_t *data = (const uint8_t *)bmp->data;
    size_t total_bytes = bmp->dib_header.bmp_bytesz;

    // read inversion flags from bytes 0-3
    bool invert_pattern[4];
    for (int i = 0; i < 4; i++) {
        invert_pattern[i] = (data[i] & 1) != 0;
    }

    //create a copy of data and apply inversions from byte 3 onwards
    uint8_t *data_inverted = (uint8_t *)malloc(total_bytes);
    if (!data_inverted) {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        return;
    }
    
    for (size_t i = 0; i < total_bytes; i++) {
        data_inverted[i] = data[i];
        
        if (i >= 3) {  // Apply inversion from byte 3 onwards
            uint8_t pattern = get_pattern(data_inverted[i]);
            if (invert_pattern[pattern]) {
                data_inverted[i] ^= 1;
            }
        }
    }
    
    // skipping RED channel
    lsb1_extract_no_red(data_inverted, out, msg_len, 4);
    
    free(data_inverted);

}

// helper function: embed using LSB1 but skipping RED channel
static void lsb1_embed_no_red(uint8_t *data, const uint8_t *message, size_t message_length, size_t start_byte) {
    size_t message_idx = 0;
    int bit_in_byte = 0;
    uint8_t current_char = message[0];
    
    // In BMP format: byte order is B, G, R, B, G, R, ...
    // start_byte = 4, so 4 % 3 = 1, which is GREEN
    // Color sequence from start_byte=4: G(4), R(5-skip), B(6), G(7), R(8-skip), ...
    
    size_t data_idx = start_byte;
    int color_phase = start_byte % 3;  // 0=B, 1=G, 2=R
    
    while (message_idx < message_length) {
        // Skip RED channel
        if (color_phase == 2) {
            data_idx++;
            color_phase = (color_phase + 1) % 3;
            continue;
        }
        
        int bit_to_embed = (current_char >> (7 - bit_in_byte)) & 1;
        
        data[data_idx] = (data[data_idx] & 0xFE) | bit_to_embed;
        
        bit_in_byte++;
        data_idx++;
        color_phase = (color_phase + 1) % 3;
        
        if (bit_in_byte == 8) {
            message_idx++;
            if (message_idx >= message_length) {
                break;
            }
            current_char = message[message_idx];
            bit_in_byte = 0;
        }
    }
}

// helper function: extract using LSB1 but skipping RED channel
static void lsb1_extract_no_red(const uint8_t *data, uint8_t *out, size_t msg_len, size_t start_byte) {
    size_t message_idx = 0;
    int bit_in_byte = 0;
    uint8_t current_char = 0;
    
    size_t data_idx = start_byte;
    int color_phase = start_byte % 3;  // 0=B, 1=G, 2=R
    
    while (message_idx < msg_len) {
        // Skip RED channel 
        if (color_phase == 2) {
            data_idx++;
            color_phase = (color_phase + 1) % 3;
            continue;
        }
        int extracted_bit = data[data_idx] & 1;
        current_char = (current_char << 1) | extracted_bit;
        
        bit_in_byte++;
        data_idx++;
        color_phase = (color_phase + 1) % 3;
        
        if (bit_in_byte == 8) {
            out[message_idx] = current_char;
            message_idx++;
            bit_in_byte = 0;
            current_char = 0;
        }
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
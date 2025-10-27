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
<<<<<<< HEAD
    // analysing capacity (we need 4 extra bytes  or pattern flags)
    if((message_length * 4 + 8) > original_bmp->dib_header.bmp_bytesz){
        fprintf(stderr, "Error: Message length exceeds bmp capacity. Max is %d\n",original_bmp->dib_header.bmp_bytesz);
=======
    // analysing capacity (we need 8 extra bytes: 4 for message length + 4 for pattern flags)
    // if((message_length * 8 + 8) > original_bmp->dib_header.bmp_bytesz){
    //     fprintf(stderr, "Error: Message length exceeds bmp capacity. Max is %d\n",original_bmp->dib_header.bmp_bytesz);
    //     return 1;
    // }

    // analyzing capacity: we need 4 bytes for pattern flags 
    // message will be embedded starting from byte 4
    // but we only use GREEN AND BLUE channels (not RED) so effective capacity is 2/3
    size_t effective_capacity = (original_bmp->dib_header.bmp_bytesz * 2) / 3;
    if ((message_length * 8 + 4 * 8) > effective_capacity) {
        fprintf(stderr, "Error: Message length exceeds bmp capacity. Max is %d\n", effective_capacity);
>>>>>>> 4043f341524f63d762467e001159e9294e16cf90
        return 1;
    }

    // we first encrypt using "lsb1", whilst also keeping track of the different pattern groups
    // we must skip the first 8 bytes: 4 for message length + 4 for pattern flags
<<<<<<< HEAD
    uint8_t * data_start = (uint8_t *)original_bmp->data;
    uint8_t * lsb1_data = data_start + 4; // we advance the pointer 4 bytes, the payload is after the pattern
    size_t bit_idx = 0;
    // we "group" by 1st2nd bit pattern
    // possible gorups:
    // 00 -> 0
    // 01 -> 1
    // 10 -> 2
    // 11 -> 3
=======
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

    // step 2: apply lsb1 starting from byte 4, skipping RED channel (every 3rd byte starting from byte 2)
    lsb1_embed_no_red(data, message, message_length, 4); 

    // step 3: count patterns and changes for each pattern
>>>>>>> 4043f341524f63d762467e001159e9294e16cf90
    pattern_group groups[4] = {0};
    for (size_t i = 0; i < total_bytes; i++) {
        uint8_t pattern = get_pattern(data[i]);

        groups[pattern].total++;

        if ((data[i] & 1) != original_lsbs[i] & 1) {
            groups[pattern].count++;
        }

        // store reference to this byte
        append_ref(&groups[pattern], &data[i], i);
    }

    free(original_lsbs);

    // step 4: decide which patterns to invert
    bool invert_pattern[4] = {false};
    for (int p = 0; p < 4; p++) {
        if (groups[p].count * 2 > groups[p].total) {
            invert_pattern[p] = true;
        }
    }

    // step 5: apply inversions from byte 3 onwards (PATTERN_QTY - 1)
    for (int p = 0; p < 4; p++) {
<<<<<<< HEAD
        if (groups[p].count * 2> groups[p].total) { // if pattern p marked for inversion
            data_start[p] = (data_start[p] & 0xFE) | 1; // store flag in bytes 4-7
=======
        if (invert_pattern[p]) {
>>>>>>> 4043f341524f63d762467e001159e9294e16cf90
            struct byte_ref *node = groups[p].items;
            while (node) {
                if (node->index >= 3) {  // Only invert from byte 3 onwards
                    *(node->ptr) ^= 1;
                }
                node = node->next;
            }
        }
    }
    // step 6: Store inversion flags in bytes 0-3
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



    // uint8_t * lsb1_data = data_start + 8; // we advance the pointer 8 bytes
    // size_t bit_idx = 0;
    // we "group" by 1st2nd bit pattern
    // possible gorups:
    // 00 -> 0
    // 01 -> 1
    // 10 -> 2
    // 11 -> 3
    // pattern_group groups[4] = {0};
    // for (size_t i = 0; i < message_length; i++) {
    //     for (int bit = 7; bit >= 0; bit--) { // for each bit in a message - byte...
    //         uint8_t bit_value = (message[i] >> bit) & 1;
    //         uint8_t original_lsb = lsb1_data[bit_idx] & 1;

    //         // WRONG!!! this would analyze the LSB of two consecutive bytes !!
    //         // uint8_t b1 = lsb1_data[bit_idx] & 1;
    //         // uint8_t b2 = lsb1_data[i+1] & 1;
    //         // uint8_t pattern = (b1 << 1) | b2; 
            
    //         uint8_t pattern = get_pattern(lsb1_data[bit_idx]);

    //         // we add to the corresponding group
    //         pattern_group *g = &groups[pattern];
    //         g->total++;

    //         if (bit_value != original_lsb)
    //             g->count++;

    //         // add byte reference to this group
    //         append_ref(g, &lsb1_data[bit_idx], bit_idx);

    //         lsb1_data[bit_idx] = (lsb1_data[bit_idx] & 0xFE) | bit_value; 
    //         bit_idx++;
    //     }
    // }
    // now we must decide which to invert 
    // for (int p = 0; p < 4; p++) {
    //     if (groups[p].count * 2> groups[p].total) { // if pattern p marked for inversion
    //         data_start[4 + p] = (data_start[4 + p] & 0xFE) | 1; // store flag in bytes 4-7
    //         struct byte_ref *node = groups[p].items;
    //         while (node) {
    //             *(node->ptr) ^= 1; // flip LSB in-place
    //             node = node->next;
    //         }
    //     }
    //    //else {
    //      //   data_start[4 + p] = data_start[4 + p] & 0xFE; // it would already be 0 but just in case...
    //     //}
    // }

}

void lsbi_extract(const struct bmp_image_t *bmp, uint8_t *out, size_t msg_len) {
<<<<<<< HEAD
    const uint8_t *data_start = (const uint8_t *)bmp->data;
    const uint8_t *lsb1_data = data_start + 4;
    size_t bit_idx = 0;
=======
>>>>>>> 4043f341524f63d762467e001159e9294e16cf90

    const uint8_t *data = (const uint8_t *)bmp->data;
    size_t total_bytes = bmp->dib_header.bmp_bytesz;

    // step 1: read inversion flags from bytes 0-3
    bool invert_pattern[4];
    for (int i = 0; i < 4; i++) {
<<<<<<< HEAD
        pattern_flags[i] = data_start[i] & 1; // 1 ->pattern was inverted (read from bytes 4-7)
=======
        invert_pattern[i] = (data[i] & 1) != 0;
>>>>>>> 4043f341524f63d762467e001159e9294e16cf90
    }

    //step 2: create a copy of data and apply inversions from byte 3 onwards
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
    
    // step 3: extract message starting from byte 4, skipping RED channel
    lsb1_extract_no_red(data_inverted, out, msg_len, 4);
    
    free(data_inverted);

    // // const uint8_t *data_start = (const uint8_t *)bmp->data;
    // const uint8_t *lsb1_data = data_start + 8;
    // size_t bit_idx = 0;

    // uint8_t pattern_flags[4] = {0};

    // for (int i = 0; i < 4; i++) {
    //     pattern_flags[i] = data_start[4 + i] & 1; // 1 ->pattern was inverted (read from bytes 4-7)
    // }

    // // extracting message bits
    // for (size_t i = 0; i < msg_len; i++) {
    //     uint8_t current_byte = 0;

    //     for (int bit = 7; bit >= 0; bit--) {
    //         //  pattern from current cover data
    //         //uint8_t b1 = lsb1_data[bit_idx] & 1;
    //         //uint8_t b2 = lsb1_data[bit_idx + 1] & 1;
    //         //uint8_t pattern = (b1 << 1) | b2; // pattern = 00,01,10,11 -> 0..3

    //         uint8_t pattern = get_pattern(lsb1_data[bit_idx]);
    //         uint8_t extracted_bit = lsb1_data[bit_idx] & 1;

    //         // if this pattern was inverted → undo it
    //         if (pattern_flags[pattern]) {
    //             extracted_bit ^= 1;
    //         }

    //         current_byte |= (extracted_bit << bit);

    //         bit_idx++;
    //     }
    //     out[i] = current_byte;
    // }
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
        // Skip RED channel (phase 2)
        if (color_phase == 2) {
            data_idx++;
            color_phase = (color_phase + 1) % 3;
            continue;
        }
        
        // Extract bit to embed (MSB first)
        int bit_to_embed = (current_char >> (7 - bit_in_byte)) & 1;
        
        // Embed in LSB
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
        // Skip RED channel (phase 2)
        if (color_phase == 2) {
            data_idx++;
            color_phase = (color_phase + 1) % 3;
            continue;
        }
        
        // Extract LSB
        int extracted_bit = data[data_idx] & 1;
        
        // Build byte (MSB first)
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
#include "test.h"
#include "../include/file_utils.h"
#include "../include/lsb4.h"

void lsb4_steganography_file_test(
    const char *original_path,
    const char *output_bmp_path,
    const char *secret_msg_path,
    const char *extracted_msg_path) {
        
    struct bmp_image_t *bmp = load_bmp_new(original_path);
    if (!bmp) {
        fprintf(stderr, "Error: could not load BMP '%s'\n", original_path);
        return;
    }
    
    size_t msg_len = 0;
    uint8_t *msg_buf = read_file(secret_msg_path, &msg_len);
    if (!msg_buf) {
        bmp_image_free(bmp);
        return;
    }
    
    printf("Loaded message file '%s' (%zu bytes)\n", secret_msg_path, msg_len);
    
    // Ocultar
    if (stego_lsb4(bmp, msg_buf, msg_len) != 0) {
        fprintf(stderr, "Error: LSB4 encoding failed\n");
        free(msg_buf);
        bmp_image_free(bmp);
        return;
    }
    
    printf("Message encoded successfully\n");
    
    bmp_write(bmp, output_bmp_path);
    printf("Stego BMP written to '%s'\n", output_bmp_path);
    
    free(msg_buf);
    bmp_image_free(bmp);
    
    struct bmp_image_t *stego_bmp = load_bmp_new(output_bmp_path);
    if (!stego_bmp) {
        fprintf(stderr, "Error: could not reload stego BMP '%s'\n", output_bmp_path);
        return;
    }
    
    uint8_t *extracted_buf = calloc(msg_len, sizeof(uint8_t));
    if (!extracted_buf) {
        fprintf(stderr, "Memory allocation failed\n");
        bmp_image_free(stego_bmp);
        return;
    }
    
    lsb4_extract(stego_bmp, extracted_buf, msg_len);
    bmp_image_free(stego_bmp);
    
    if (write_file(extracted_msg_path, extracted_buf, msg_len) == 0) {
        printf("Extracted message saved to '%s'\n", extracted_msg_path);
    } else {
        fprintf(stderr, "Error: could not write extracted message\n");
    }
    
    uint8_t *orig_check = read_file(secret_msg_path, &msg_len);
    if (orig_check && memcmp(orig_check, extracted_buf, msg_len) == 0) {
        printf("✅ SUCCESS: Extracted data matches original\n");
    } else {
        printf("❌ FAIL: Extracted data differs from original\n");
    }
    
    free(orig_check);
    free(extracted_buf);
}
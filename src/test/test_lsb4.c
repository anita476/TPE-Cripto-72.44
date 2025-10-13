#include "test.h"
#include "../include/lsb4.h"

void lsb4_test(const char *original_path, const char *output_path) {
    
    struct bmp_image_t *bmp = load_bmp_new(original_path);
    if (!bmp) {
        fprintf(stderr, "Error: could not load BMP file '%s'\n", original_path);
        return;
    }
    
    printf("Loaded BMP: %s (%dx%d, %d bpp)\n",
           original_path, 
           bmp->dib_header.width, 
           bmp->dib_header.height, 
           bmp->dib_header.bitspp);
    
    size_t capacity_bytes = bmp->dib_header.bmp_bytesz / 2;
    printf("LSB4 Capacity: %zu bytes\n", capacity_bytes);

    const char *secret_msg = "LSB4 test message!";
    size_t msg_len = strlen(secret_msg) + 1; // include '\0'
    
    printf("Message to hide: \"%s\" (%zu bytes)\n", secret_msg, msg_len);
    
    if (stego_lsb4(bmp, (uint8_t *)secret_msg, msg_len) != 0) {
        fprintf(stderr, "Error: LSB4 encoding failed\n");
        bmp_image_free(bmp);
        return;
    }
    
    printf("Message encoded successfully\n");


    bmp_write(bmp, output_path);
    printf("Stego image written to: %s\n", output_path);
    
    bmp_image_free(bmp);
    
    struct bmp_image_t *bmp_stego = load_bmp_new(output_path);
    if (!bmp_stego) {
        fprintf(stderr, "Error: could not reload stego BMP\n");
        return;
    }
    
    uint8_t *extracted_msg = calloc(msg_len, sizeof(uint8_t));
    if (!extracted_msg) {
        fprintf(stderr, "Memory allocation failed\n");
        bmp_image_free(bmp_stego);
        return;
    }
    
    lsb4_extract(bmp_stego, extracted_msg, msg_len);
    printf("Extracted message: \"%s\"\n", extracted_msg);
    
    if (strcmp(secret_msg, (char *)extracted_msg) == 0) {
        printf("✅ SUCCESS: Message matches!\n");
    } else {
        printf("❌ FAIL: Message mismatch\n");
        printf("   Expected: \"%s\"\n", secret_msg);
        printf("   Got:      \"%s\"\n", extracted_msg);
    }
    
    free(extracted_msg);
    bmp_image_free(bmp_stego);
}
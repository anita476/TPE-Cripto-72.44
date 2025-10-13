#include "test.h"
#include "../include/improved_lsb1.h"

static uint8_t *read_file(const char *path, size_t *out_size);
static int write_file(const char *path, const uint8_t *data, size_t size);

void improved_lsb1_steganography_file_test(const char *original_path,
    const char *output_bmp_path,
    const char *secret_msg_path,
    const char *extracted_msg_path) {

    // Load the BMP
    struct bmp_image_t *bmp = load_bmp_new(original_path);
    if (!bmp) {
        fprintf(stderr, "Error: could not load BMP '%s'\n", original_path);
        return;
    }

    // Load the message file
    size_t msg_len = 0;
    uint8_t *msg_buf = read_file(secret_msg_path, &msg_len);
    if (!msg_buf) {
        bmp_image_free(bmp);
        return;
    }

    printf("Loaded message file '%s' (%zu bytes)\n", secret_msg_path, msg_len);

    // Encode the message
    if (stego_improved_lsb1(bmp, msg_buf, msg_len) != 0) {
        fprintf(stderr, "Error: LSB1 encoding failed.\n");
        free(msg_buf);
        bmp_image_free(bmp);
        return;
    }
    printf("Message encoded successfully.\n");

    // Write the stego image
    bmp_write(bmp, output_bmp_path);
    printf("Stego BMP written to '%s'\n", output_bmp_path);

    // Cleanup
    free(msg_buf);
    bmp_image_free(bmp);

    // Reload the stego image
    struct bmp_image_t *stego_bmp = load_bmp_new(output_bmp_path);
    if (!stego_bmp) {
        fprintf(stderr, "Error: could not reload stego BMP '%s'\n", output_bmp_path);
        return;
    }

    // Extract the message
    uint8_t *extracted_buf = calloc(msg_len, sizeof(uint8_t));
    if (!extracted_buf) {
        fprintf(stderr, "Memory allocation failed for extracted buffer.\n");
        bmp_image_free(stego_bmp);
        return;
    }

    improved_lsb1_extract(stego_bmp, extracted_buf, msg_len);
    bmp_image_free(stego_bmp);

    // Save the extracted message to a file
    if (write_file(extracted_msg_path, extracted_buf, msg_len) == 0)
        printf("Extracted message saved to '%s'\n", extracted_msg_path);
    else
        fprintf(stderr, "Error: could not write extracted message.\n");


    uint8_t *orig_check = read_file(secret_msg_path, &msg_len);
    if (orig_check && memcmp(orig_check, extracted_buf, msg_len) == 0)
        printf("✅ SUCCESS: Extracted data matches original.\n");
    else
        printf("❌ FAIL: Extracted data differs from original.\n");

    free(orig_check);
    free(extracted_buf);
}


static uint8_t *read_file(const char *path, size_t *out_size) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Error: cannot open input file '%s'\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if (size <= 0) {
        fprintf(stderr, "Error: file '%s' is empty or unreadable.\n", path);
        fclose(f);
        return NULL;
    }

    uint8_t *buf = malloc(size);
    if (!buf) {
        fprintf(stderr, "Error: could not allocate %ld bytes.\n", size);
        fclose(f);
        return NULL;
    }

    fread(buf, 1, size, f);
    fclose(f);
    *out_size = (size_t)size;
    return buf;
}

static int write_file(const char *path, const uint8_t *data, size_t size) {
    FILE *f = fopen(path, "wb");
    if (!f) {
        fprintf(stderr, "Error: cannot open output file '%s'\n", path);
        return -1;
    }
    fwrite(data, 1, size, f);
    fclose(f);
    return 0;
}
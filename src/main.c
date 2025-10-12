#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/bmp.h"
#include "include/lsb1.h"
#include "test/test_encryption.h"

int main(int argc, char *argv[]){
    printf("Welcome to stego\n");

    printf("Encryption test:\n");
    encryption_test();
    printf("Encryption test finished\n");

    const char *input_path  = "public/pepper.bmp";
    const char *output_path = "public/pepper_stego.bmp";
    const char *message = "Hello, this is a hidden BMP world!";
    size_t msg_len = strlen(message) + 1; //  null terminator

    printf("Loading %s...\n", input_path);
    struct bmp_image_t *bmp = load_bmp_new(input_path);
    if (!bmp) return 1;

    printf("Embedding message: \"%s\"\n", message);
    stego_lsb1(bmp, (const uint8_t *)message, msg_len);

    printf("Saving to %s...\n", output_path);
    bmp_write(bmp, output_path);

    printf("Reloading %s to extract message\n", output_path);
    struct bmp_image_t *bmp2 = load_bmp_new(output_path);
    if (!bmp2) return 1;

    uint8_t *buffer = calloc(1, msg_len);
    lsb1_extract(bmp2, buffer, msg_len);

    printf("Extracted message: \"%s\"\n", buffer);

    bmp_image_free(bmp);
    bmp_image_free(bmp2);
    free(buffer);
    return 0;
}
#ifndef _TEST_H_
#define _TEST_H_
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int encryption_test(void);
void lsb1_test(const char * original_path, const char * output_path);
void lsb1_steganography_file_test(const char *original_path,
    const char *output_bmp_path,
    const char *secret_msg_path,
    const char *extracted_msg_path);

void improved_lsb1_test(const char * original_path, const char * output_path);

#endif
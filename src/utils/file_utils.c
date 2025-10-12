#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/file_utils.h"

uint8_t *read_file(const char *filename, size_t *out_len) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;
    // Find file size
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, 0, SEEK_SET);
    // Allocate buffer
    uint8_t *buf = malloc(len);
    if (!buf) { 
        fclose(f); 
        return NULL; 
    }
    // Read and close file
    size_t nread = fread(buf, 1, len, f);
    fclose(f);
    if (nread != len) { 
        free(buf); 
        return NULL; 
    }
    // Save size
    *out_len = len;
    return buf;
}

int write_file(const char *filename, const uint8_t *buf, size_t len) {
    // Open file
    FILE *f = fopen(filename, "wb");
    if (!f) return 0;
    // Write and close
    fwrite(buf, 1, len, f);
    fclose(f);
    return 1;
}

void *safe_malloc(size_t n) {
    void *p = malloc(n);
    if (!p) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    return p;
}
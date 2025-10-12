#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stddef.h>
#include <stdint.h>

uint8_t *read_file(const char *filename, size_t *out_len);

int write_file(const char *filename, const uint8_t *buf, size_t len);

void *safe_malloc(size_t n);

#endif
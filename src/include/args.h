#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>
#include "encryption.h"

typedef enum { 
    OP_NONE, 
    OP_EMBED, 
    OP_EXTRACT 
} operation_t;

typedef enum { 
    STEG_NONE, 
    STEG_LSB1, 
    STEG_LSB4, 
    STEG_LSBI 
} steg_algorithm_t;

typedef struct {
    operation_t op;
    char *in_file;
    char *bmp_file;
    char *out_file;
    steg_algorithm_t steg;
    encryption_algorithm_t algorithm;
    encryption_mode_t mode;
    char *password;
    bool use_crypto;
} args_t;

bool parse_args(int argc, char **argv, args_t *args);

void print_usage(const char *prog);

#endif // ARGS_H
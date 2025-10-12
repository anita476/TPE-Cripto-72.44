#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include "include/args.h"

static steg_algorithm_t parse_steg(const char *s) {
    if (!s) return STEG_NONE;
    if (strcasecmp(s, "LSB1") == 0) return STEG_LSB1;
    if (strcasecmp(s, "LSB4") == 0) return STEG_LSB4;
    if (strcasecmp(s, "LSBI") == 0) return STEG_LSBI;
    return STEG_NONE;
}

static encryption_algorithm_t parse_algorithm(const char *s) {
    if (!s) return ENC_AES128;
    if (strcasecmp(s, "aes128") == 0) return ENC_AES128;
    if (strcasecmp(s, "aes192") == 0) return ENC_AES192;
    if (strcasecmp(s, "aes256") == 0) return ENC_AES256;
    if (strcasecmp(s, "3des") == 0)   return ENC_3DES;
    return ENC_AES128;
}

static encryption_mode_t parse_mode(const char *s) {
    if (!s) return ENC_CBC;
    if (strcasecmp(s, "cbc") == 0) return ENC_CBC;
    if (strcasecmp(s, "ecb") == 0) return ENC_ECB;
    if (strcasecmp(s, "cfb") == 0) return ENC_CFB;
    if (strcasecmp(s, "ofb") == 0) return ENC_OFB;
    return ENC_CBC;
}

void print_usage(const char *prog) {
    printf("Usage:\n");
    printf("  %s -embed -in <file> -p <bmpfile> -out <bmpfile> -steg <LSB1|LSB4|LSBI> [-a <aes128|aes192|aes256|3des>] [-m <cbc|ecb|cfb|ofb>] [-pass <password>]\n", prog);
    printf("  %s -extract -p <bmpfile> -out <file> -steg <LSB1|LSB4|LSBI> [-a <aes128|aes192|aes256|3des>] [-m <cbc|ecb|cfb|ofb>] [-pass <password>]\n", prog);
}

bool parse_args(int argc, char **argv, args_t *args) {
    memset(args, 0, sizeof(*args));

    // Defaults
    args->algorithm = ENC_AES128;
    args->mode = ENC_CBC;
    args->steg = STEG_NONE;
    args->op = OP_NONE;
    args->use_crypto = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-embed") == 0) {
            args->op = OP_EMBED;
        } else if (strcmp(argv[i], "-extract") == 0) {
            args->op = OP_EXTRACT;
        } else if (strcmp(argv[i], "-in") == 0 && i+1 < argc) {
            args->in_file = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i+1 < argc) {
            args->bmp_file = argv[++i];
        } else if (strcmp(argv[i], "-out") == 0 && i+1 < argc) {
            args->out_file = argv[++i];
        } else if (strcmp(argv[i], "-steg") == 0 && i+1 < argc) {
            args->steg = parse_steg(argv[++i]);
        } else if (strcmp(argv[i], "-a") == 0 && i+1 < argc) {
            args->algorithm = parse_algorithm(argv[++i]);
            args->use_crypto = true;
        } else if (strcmp(argv[i], "-m") == 0 && i+1 < argc) {
            args->mode = parse_mode(argv[++i]);
            args->use_crypto = true;
        } else if (strcmp(argv[i], "-pass") == 0 && i+1 < argc) {
            args->password = argv[++i];
            args->use_crypto = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            exit(0);
        } else {
            printf("Unknown or incomplete option: %s\n", argv[i]);
            print_usage(argv[0]);
            return false;
        }
    }

    if (args->use_crypto && !args->password) {
        printf("Error: -pass <password> required for encryption/decryption\n");
        return false;
    }

    if (args->password && !args->use_crypto) {
        args->algorithm = ENC_AES128;
        args->mode = ENC_CBC;
        args->use_crypto = true;
    }

    if (args->op == OP_EMBED) {
        if (!args->in_file || !args->bmp_file || !args->out_file || args->steg == STEG_NONE) {
            printf("Missing required arguments for embed.\n");
            print_usage(argv[0]);
            return false;
        }
    } else if (args->op == OP_EXTRACT) {
        if (!args->bmp_file || !args->out_file || args->steg == STEG_NONE) {
            printf("Missing required arguments for extract.\n");
            print_usage(argv[0]);
            return false;
        }
    } else {
        printf("You must specify -embed or -extract.\n");
        print_usage(argv[0]);
        return false;
    }

    return true;
}
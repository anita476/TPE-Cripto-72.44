#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/args.h"
#include "include/bmp.h"
#include "include/lsb1.h"
#include "include/file_utils.h"
#include "include/stego_utils.h"
#include "test/test.h"

static int embed_lsb1(const args_t *args) {
    // Read input file
    size_t msg_len;
    uint8_t *msg_buf = read_file(args->in_file, &msg_len);
    if (!msg_buf) { 
        fprintf(stderr, "Error opening input file\n"); 
        return 1; 
    }

    // Pack message
    size_t plain_len;
    uint8_t *plain = pack_plain(args->in_file, msg_buf, msg_len, &plain_len);
    free(msg_buf);

    // If no cypher is used, to_hide is plain
    uint8_t *to_hide = plain;
    size_t to_hide_len = plain_len;

    // If cypher is used, encrypt and then pack
    if (args->use_crypto) {
        encryption_params_t params;
        params.algorithm = args->algorithm;
        params.mode = args->mode;
        if (!derive_key_iv(args->password, params.algorithm, params.key, &params.key_len, params.iv, &params.iv_len)) {
            fprintf(stderr, "Key derivation failed\n"); 
            free(plain); 
            return 1;
        }
        uint8_t *enc_buf = NULL;
        size_t enc_len = 0;
        if (!encrypt_buffer(plain, plain_len, &enc_buf, &enc_len, &params)) {
            fprintf(stderr, "Encryption failed\n"); 
            free(plain); 
            return 1;
        }
        free(plain);
        to_hide = pack_encrypted(enc_buf, enc_len, &to_hide_len);
        free(enc_buf);
    }

    // Load BMP
    struct bmp_image_t *bmp = load_bmp_new(args->bmp_file);
    if (!bmp) { 
        free(to_hide); 
        fprintf(stderr, "Error loading BMP\n"); 
        return 1; 
    }

    // Use LSB1
    if (stego_lsb1(bmp, to_hide, to_hide_len) != 0) {
        fprintf(stderr, "LSB1 embedding failed\n");
        bmp_image_free(bmp); 
        free(to_hide);
        return 1;
    }
    free(to_hide);

    // Save output BMP
    bmp_write(bmp, args->out_file);
    bmp_image_free(bmp);
    printf("Embedding done!\n");
    return 0;
}

static int extract_lsb1(const args_t *args) {
    // Load BMP
    struct bmp_image_t *bmp = load_bmp_new(args->bmp_file);
    if (!bmp) { 
        fprintf(stderr, "Error loading BMP\n"); 
        return 1; 
    }

    // Extract first 4 bytes to get message size
    uint8_t size_buf[4];
    lsb1_extract(bmp, size_buf, 4);
    size_t msg_len = (size_buf[0]<<24) | (size_buf[1]<<16) | (size_buf[2]<<8) | size_buf[3];

    // Validation
    if (msg_len == 0 || msg_len > bmp->dib_header.bmp_bytesz - 4) {
        fprintf(stderr, "Error: extracted msg_len (%zu) is invalid\n", msg_len);
        bmp_image_free(bmp);
        return 1;
    }

    // Extract enough bytes for [mensaje][extensión]
    size_t max_ext = 32;
    size_t total_to_extract = 4 + msg_len + max_ext;
    uint8_t *data = safe_malloc(total_to_extract);
    lsb1_extract(bmp, data, total_to_extract);
    bmp_image_free(bmp);

    // If there is encryption, decrypt
    if (args->use_crypto) {
        encryption_params_t params;
        params.algorithm = args->algorithm;
        params.mode = args->mode;
        if (!derive_key_iv(args->password, params.algorithm, params.key, &params.key_len, params.iv, &params.iv_len)) {
            fprintf(stderr, "Key derivation failed\n");
            free(data);
            return 1;
        }
        size_t enc_len = (data[0]<<24) | (data[1]<<16) | (data[2]<<8) | data[3];
        if (enc_len > total_to_extract - 4) {
            fprintf(stderr, "Error: extracted enc_len (%zu) > available (%zu)\n", enc_len, total_to_extract - 4);
            free(data);
            return 1;
        }
        uint8_t *dec = NULL;
        size_t dec_len = 0;
        if (!decrypt_buffer(data+4, enc_len, &dec, &dec_len, &params)) {
            fprintf(stderr, "Decryption failed\n");
            free(data);
            return 1;
        }
        free(data);

        // Recover size and extension from decrypted buffer
        if (dec_len < 4) {
            fprintf(stderr, "Error: decrypted length < 4\n");
            free(dec);
            return 1;
        }
        size_t real_size = (dec[0]<<24) | (dec[1]<<16) | (dec[2]<<8) | dec[3];
        if (real_size > dec_len - 4) {
            fprintf(stderr, "Error: real_size (%zu) > dec_len-4 (%zu)\n", real_size, dec_len - 4);
            free(dec);
            return 1;
        }
        char *ext = (char*)(dec + 4 + real_size);
        printf("Recovered extension: '%s'\n", ext);

        // Save output file
        char out_name[512];
        snprintf(out_name, sizeof(out_name), "%s%s", args->out_file, ext);
        if (!write_file(out_name, dec + 4, real_size)) {
            fprintf(stderr, "Error writing output file\n");
            free(dec);
            return 1;
        }
        free(dec);
        printf("Extraction done! Output: %s\n", out_name);
        return 0;
    }

    // Recover size and extension
    if (msg_len < 4) {
        fprintf(stderr, "Error: msg_len < 4\n");
        free(data);
        return 1;
    }
    size_t real_size = (data[0]<<24) | (data[1]<<16) | (data[2]<<8) | data[3];
    if (real_size > total_to_extract - 4) {
        fprintf(stderr, "Error: real_size (%zu) > available (%zu)\n", real_size, total_to_extract - 4);
        free(data);
        return 1;
    }
    char *ext = (char*)(data + 4 + real_size);
    printf("Recovered extension: '%s'\n", ext);

    // Save output file
    char out_name[512];
    snprintf(out_name, sizeof(out_name), "%s%s", args->out_file, ext);
    if (!write_file(out_name, data + 4, real_size)) {
        fprintf(stderr, "Error writing output file\n");
        free(data);
        return 1;
    }
    free(data);
    printf("Extraction done! Output: %s\n", out_name);
    return 0;
}

static int embed_dispatch(const args_t *args) {
    switch (args->steg) {
        case STEG_LSB1: return embed_lsb1(args);
        // case STEG_LSB4: return embed_lsb4(args); // TODO
        // case STEG_LSBI: return embed_lsbi(args); // TODO
        default:
            fprintf(stderr, "Steganography algorithm not implemented.\n");
            return 1;
    }
}

static int extract_dispatch(const args_t *args) {
    switch (args->steg) {
        case STEG_LSB1: return extract_lsb1(args);
        // case STEG_LSB4: return extract_lsb4(args); // TODO
        // case STEG_LSBI: return extract_lsbi(args); // TODO
        default:
            fprintf(stderr, "Steganography algorithm not implemented.\n");
            return 1;
    }
}

int main(int argc, char **argv) {
    printf("Welcome to stego\n");

    args_t args;
    if (!parse_args(argc, argv, &args)) {
        return 1;
    }

    if (args.op == OP_EMBED)
        return embed_dispatch(&args);
    else if (args.op == OP_EXTRACT)
        return extract_dispatch(&args);

    print_usage(argv[0]);
    return 1;
}
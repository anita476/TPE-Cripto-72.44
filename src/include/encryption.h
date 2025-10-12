#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    ENC_AES128,
    ENC_AES192,
    ENC_AES256,
    ENC_3DES
} encryption_algorithm_t;

typedef enum {
    ENC_ECB,
    ENC_CFB,
    ENC_OFB,
    ENC_CBC
} encryption_mode_t;

typedef struct {
    encryption_algorithm_t algorithm;
    encryption_mode_t mode;
    uint8_t key[32];
    uint8_t iv[16];
    size_t key_len;
    size_t iv_len;
} encryption_params_t;

int derive_key_iv(const char *password, encryption_algorithm_t algorithm, uint8_t *key, size_t *key_len, uint8_t *iv, size_t *iv_len);

int encrypt_buffer(const uint8_t *in, size_t in_len, uint8_t **out, size_t *out_len, const encryption_params_t *params);

int decrypt_buffer(const uint8_t *in, size_t in_len, uint8_t **out, size_t *out_len, const encryption_params_t *params);

#endif // ENCRYPTION_H
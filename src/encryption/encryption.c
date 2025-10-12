#include "../include/encryption.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>

#define PBKDF2_SALT "\x00\x00\x00\x00\x00\x00\x00\x00"
#define PBKDF2_SALT_LEN 8
#define PBKDF2_ITER 10000

static const EVP_CIPHER *get_cipher(encryption_algorithm_t algorithm, encryption_mode_t mode) {
    switch (algorithm) {
        case ENC_AES128:
            switch (mode) {
                case ENC_ECB: return EVP_aes_128_ecb();
                case ENC_CFB: return EVP_aes_128_cfb8();
                case ENC_OFB: return EVP_aes_128_ofb();
                case ENC_CBC: return EVP_aes_128_cbc();
            }
            break;
        case ENC_AES192:
            switch (mode) {
                case ENC_ECB: return EVP_aes_192_ecb();
                case ENC_CFB: return EVP_aes_192_cfb8();
                case ENC_OFB: return EVP_aes_192_ofb();
                case ENC_CBC: return EVP_aes_192_cbc();
            }
            break;
        case ENC_AES256:
            switch (mode) {
                case ENC_ECB: return EVP_aes_256_ecb();
                case ENC_CFB: return EVP_aes_256_cfb8();
                case ENC_OFB: return EVP_aes_256_ofb();
                case ENC_CBC: return EVP_aes_256_cbc();
            }
            break;
        case ENC_3DES:
            switch (mode) {
                case ENC_ECB: return EVP_des_ede3_ecb();
                case ENC_CFB: return EVP_des_ede3_cfb8();
                case ENC_OFB: return EVP_des_ede3_ofb();
                case ENC_CBC: return EVP_des_ede3_cbc();
            }
            break;
    }
    return NULL;
}

int derive_key_iv(const char *password, encryption_algorithm_t algorithm, uint8_t *key, size_t *key_len, uint8_t *iv, size_t *iv_len) {
    int total_len = 0;
    switch (algorithm) {
        case ENC_AES128: *key_len = 16; *iv_len = 16; total_len = 32; break;
        case ENC_AES192: *key_len = 24; *iv_len = 16; total_len = 40; break;
        case ENC_AES256: *key_len = 32; *iv_len = 16; total_len = 48; break;
        case ENC_3DES:   *key_len = 24; *iv_len = 8;  total_len = 32; break;
        default: return 0;
    }
    uint8_t buf[48];
    if (!PKCS5_PBKDF2_HMAC(password, strlen(password), (const uint8_t*)PBKDF2_SALT, PBKDF2_SALT_LEN, PBKDF2_ITER, EVP_sha256(), total_len, buf)) {
        return 0;
    }
    memcpy(key, buf, *key_len);
    memcpy(iv, buf + *key_len, *iv_len);
    return 1;
}

int encrypt_buffer(const uint8_t *in, size_t in_len, uint8_t **out, size_t *out_len, const encryption_params_t *params) {
    const EVP_CIPHER *cipher = get_cipher(params->algorithm, params->mode);
    if (!cipher) return 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return 0;

    int block_size = EVP_CIPHER_block_size(cipher);
    int max_len = in_len + block_size;

    *out = malloc(max_len);
    if (!*out) { 
        EVP_CIPHER_CTX_free(ctx); 
        return 0; 
    }

    int len = 0, total = 0;
    if (!EVP_EncryptInit_ex(ctx, cipher, NULL, params->key, params->iv)) goto err;
    if (!EVP_EncryptUpdate(ctx, *out, &len, in, in_len)) goto err;
    total = len;
    if (!EVP_EncryptFinal_ex(ctx, *out + len, &len)) goto err;
    total += len;

    *out_len = total;
    EVP_CIPHER_CTX_free(ctx);
    return 1;

    err:
        EVP_CIPHER_CTX_free(ctx);
        free(*out);
        *out = NULL;
        return 0;
}

int decrypt_buffer(const uint8_t *in, size_t in_len, uint8_t **out, size_t *out_len, const encryption_params_t *params) {
    const EVP_CIPHER *cipher = get_cipher(params->algorithm, params->mode);
    if (!cipher) return 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return 0;

    *out = malloc(in_len);
    if (!*out) { 
        EVP_CIPHER_CTX_free(ctx); 
        return 0; 
    }

    int len = 0, total = 0;
    if (!EVP_DecryptInit_ex(ctx, cipher, NULL, params->key, params->iv)) goto err;
    if (!EVP_DecryptUpdate(ctx, *out, &len, in, in_len)) goto err;
    total = len;
    if (!EVP_DecryptFinal_ex(ctx, *out + len, &len)) goto err;
    total += len;

    *out_len = total;
    EVP_CIPHER_CTX_free(ctx);
    return 1;

err:
    EVP_CIPHER_CTX_free(ctx);
    free(*out);
    *out = NULL;
    return 0;
}
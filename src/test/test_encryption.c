#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "test.h"
#include "../include/encryption.h"

const char *algo_names[] = {"AES128", "AES192", "AES256", "3DES"};
const char *mode_names[] = {"ECB", "CFB", "OFB", "CBC"};

int encryption_test(void) {
    const char *password = "mypassword";
    const char *message = "Hello secret world!";
    size_t message_len = strlen(message);

    for (int a = ENC_AES128; a <= ENC_3DES; ++a) {
        for (int m = ENC_ECB; m <= ENC_CBC; ++m) {
            encryption_params_t params;
            params.algorithm = (encryption_algorithm_t)a;
            params.mode = (encryption_mode_t)m;

            if (!derive_key_iv(password, params.algorithm, params.key, &params.key_len, params.iv, &params.iv_len)) {
                printf("[%s-%s] Error deriving key/iv\n", algo_names[a], mode_names[m]);
                continue;
            }

            uint8_t *encrypted = NULL;
            size_t encrypted_len = 0;
            if (!encrypt_buffer((const uint8_t*)message, message_len, &encrypted, &encrypted_len, &params)) {
                printf("[%s-%s] Error encrypting\n", algo_names[a], mode_names[m]);
                continue;
            }

            uint8_t *decrypted = NULL;
            size_t decrypted_len = 0;
            if (!decrypt_buffer(encrypted, encrypted_len, &decrypted, &decrypted_len, &params)) {
                printf("[%s-%s] Error decrypting\n", algo_names[a], mode_names[m]);
                free(encrypted);
                continue;
            }

            if (decrypted_len != message_len || memcmp(message, decrypted, message_len) != 0) {
                printf("[%s-%s] ERROR! Decrypted text does not match the original\n", algo_names[a], mode_names[m]);
            } else {
                printf("[%s-%s] Success! Decrypted text matches: %s\n", algo_names[a], mode_names[m], decrypted);
            }

            free(encrypted);
            free(decrypted);
        }
    }
    return 0;
}
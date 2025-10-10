#include "../include/bmp.h"
#include <stdlib.h>

struct bmp_image_t * load_bmp_new(const char * filepath){
    FILE * file = fopen(filepath, "rb");
    if(file == NULL) {
        fprintf(stderr, "Error: failed to open file %s",filepath);
        return NULL;
    }
    struct bmp_image_t * bmp = malloc(sizeof(struct bmp_image_t));

    if(bmp == NULL) {
        fclose(file);
        fprintf(stderr, "Error: Failed to allocate memory for bmp in %s",filepath);
        return NULL;
    }
}

void bmp_image_free(struct bmp_image_t * bmp){
    if(bmp == NULL) return;
    free(bmp->data);
    bmp->data = NULL;
    free(bmp);
    bmp = NULL;
}
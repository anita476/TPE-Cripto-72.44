#include "../include/bmp.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// number of bytes used for a row
// bmp rows are usually aligned to 4-byte boundaries, so padding is included
static inline uint32_t row_stride(uint32_t width) {
    return (width * 3 + 3) & ~3;
}

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

    // read the headers
    fread(&bmp->file_header, sizeof(struct bmpfile_header), 1, file);
    fread(&bmp->dib_header,sizeof(struct bmp_dib_v3_header_t),1,file);

    // validations
    if(bmp->file_header.bfType != 0x4D42){
        fclose(file);
        fprintf(stderr, "Error: provided file is not a BMP file\n");
        free(bmp);
        return NULL;
    }
    if(bmp->dib_header.bitspp != 24){
        fclose(file);
        fprintf(stderr, "Error: Only 24 bit pixels bmp images supported\n");
        free(bmp);
        return NULL;
    }
    if(bmp->dib_header.compress_type != 0){
        fclose(file);
        fprintf(stderr, "Error: no compression allowed");
        free(bmp);
        return NULL;
    }

    // compute the size of the data
    uint32_t datasz = bmp->dib_header.bmp_bytesz;
    // if uncompressed it could be zero, so we can normalize it in memory
    if(datasz == 0 ){
        datasz = row_stride(bmp->dib_header.width) * abs(bmp->dib_header.height);
        bmp->dib_header.bmp_bytesz = datasz;
    }

    bmp->data = malloc(datasz);
    if(!bmp->data) {
        fclose(file);
        fprintf(stderr, "Error: could not allocate memory for bmp data");
        free(bmp);
        return NULL;
    }

    fseek(file,bmp->file_header.bmp_offset,SEEK_SET);
    int res = fread(bmp->data,1, datasz, file);
    if(res == -1){
        fclose(file);
        fprintf(stderr, "Error: could not read bmp file data");
        free(bmp->data);
        free(bmp);
        return NULL;
    }
    fclose(file);

    return bmp;
}

void bmp_write(const struct bmp_image_t *bmp, const char * path){
    FILE *f = fopen(path, "wb");
    if (!f) return;    
    fwrite(&bmp->file_header, sizeof(bmp->file_header), 1, f);
    fwrite(&bmp->dib_header, sizeof(bmp->dib_header), 1, f);
    fwrite(bmp->data, 1, bmp->dib_header.bmp_bytesz, f);
    fclose(f);
}

void bmp_image_free(struct bmp_image_t * bmp){
    if(bmp == NULL) return;
    free(bmp->data);
    bmp->data = NULL;
    free(bmp);
    bmp = NULL;
}
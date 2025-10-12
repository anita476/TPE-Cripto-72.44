#include <stdio.h>
#include "include/bmp.h"

int main(int argc, char *argv[]){
    printf("Welcome to stego\n");


    struct bmp_image_t *img = load_bmp_new("public/pepper.bmp");
    if (!img) return 1;

    printf("Loaded BMP:\n");
    printf("  Size:   %dx%d\n", img->dib_header.width, img->dib_header.height);
    printf("  Depth:  %d bits per pixel\n", img->dib_header.bitspp);

    bmp_image_free(img);
    return 0;
}
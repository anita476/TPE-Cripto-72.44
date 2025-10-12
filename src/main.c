#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/bmp.h"
#include "include/lsb1.h"
#include "test/test.h"

int main(int argc, char *argv[]){
    printf("Welcome to stego\n");

    printf("\n===============================Encryption test:===========================================\n");
    encryption_test();
    printf("Encryption test finished\n");


    printf("\n===============================LSB1 test:=================================================\n");

    char * input = "public/pepper.bmp";
    char * output = "public/pepper_stego.bmp";
    lsb1_test(input,output);
    printf("LSB1 test finished\n");

}
#include "test.h"


int main (void) {
   
    printf("\n===============================STARTING TESTS=============================================\n");
    printf("\n===============================Encryption test:===========================================\n");
    encryption_test();
    printf("Encryption test finished\n");


    printf("\n===============================LSB1 test:=================================================\n");

    char * input = "public/pepper.bmp";
    char * output = "public/pepper_stego.bmp";
    lsb1_test(input,output);
    printf("LSB1 test finished\n");

    printf("\n==============================LSB1 file test:=============================================\n");
    lsb1_steganography_file_test(input,output,"public/test_message.txt","public/test_message_stego.txt");
    printf("LSB1 file test finished\n");

    return 0;
}
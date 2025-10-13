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

    printf("\n==============================LSB4 Test:===================================================\n");
    char *output_lsb4 = "public/pepper_stego_lsb4.bmp";
    lsb4_test(input, output_lsb4);
    
    printf("\n===============================LSB4 File Test:=============================================\n");
    lsb4_steganography_file_test(
        input,
        output_lsb4,
        "public/test_message.txt",
        "public/test_message_stego_lsb4.txt"
    );

    printf("\n===============================TESTS COMPLETE:=============================================\n");

    return 0;
}
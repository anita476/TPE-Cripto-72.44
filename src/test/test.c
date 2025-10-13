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
    // todo add a huge file to stresstest 
    printf("\n==============================LSB1 file test:=============================================\n");
    lsb1_steganography_file_test("public/airplane.bmp","public/airplane_stego.bmp","public/random_file","public/random_file_stego");
    printf("LSB1 file test finished\n");

    printf("\n==============================Imrpoved LSB1 test===========================================\n");
    improved_lsb1_test("public/baboon.bmp", "public/baboon_stego.bmp");
    printf("Improved LSB1 test finished\n");

    //Obs! To create the file run:
    // dd if=/dev/urandom of=public/random_file bs=1K count=10
    printf("\n==============================Imrpoved LSB1 file test======================================\n");
    improved_lsb1_steganography_file_test("public/baboon.bmp", "public/baboon_stego.bmp","public/random_file","public/random_file_stego");
    printf("Improved LSB1 file test finished\n");

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
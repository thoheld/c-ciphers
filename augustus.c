#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "caesar.h"

char *augustus_encrypt(char *plain, char *key) {
	char* cipher;
	cipher = caesar_encrypt(plain, key); // Step 1: run through normal caesar cipher

	char* char_ptr;
	char* curr_char = calloc(2, sizeof(char)); // set up to send individual chars
	*(curr_char+1) = '\0';
	char* curr_key_digit = calloc(2, sizeof(char)); // set up to send individual digits of key
	*(curr_key_digit+1) = '\0';
	

	for (int i = 0; i < strlen(cipher); i++) { // Step 2: for every char in message
		*curr_char = *(cipher+i);
		*curr_key_digit = *(key+(i%strlen(key)));
		char_ptr = caesar_encrypt(curr_char, curr_key_digit);
		*(cipher + i) = *char_ptr;
		free(char_ptr);
	}

	free(curr_char);
	free(curr_key_digit);
	return cipher;
}

char *augustus_decrypt(char *cipher, char *key) {
    char *initDec;
    fflush(stdout);
    initDec = caesar_decrypt(cipher, key);

    int keyPos = 0;
    char *finalDec = malloc(sizeof(cipher));
    char temp;
    for (int i = 0; i < strlen(cipher); i++) {
        temp = key[keyPos];
        if (initDec[i] == 32) {
            finalDec[i] = initDec[i];
        } else {
            finalDec[i] = caesar_decrypt_char(initDec[i], atoi(&temp));
        }
        if (keyPos == (strlen(key)-2)) {
            keyPos = 0;
        } else {
            keyPos++;
        }
    }
    return finalDec;

}



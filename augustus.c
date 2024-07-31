#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "caesar.h"



/*
 * Encrypts a string using the augustus cipher.
 * Returns pointer to encrypted string.
 */
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



/*
 * Decrypts a string using the augustus cipher.
 * Returns pointer to decrypted string.
 */
char *augustus_decrypt(char *cipher, char *key) {
	char* plain = calloc(strlen(cipher)+1, sizeof(char));
	strcpy(plain, cipher); // initialize plain with cipher message

	char* char_ptr;
	char* curr_char = calloc(2, sizeof(char)); // set up to send individual chars
	*(curr_char+1) = '\0';
	char* curr_key_digit = calloc(2, sizeof(char)); // set up to send individual digits of key
	*(curr_key_digit+1) = '\0';
	
	for (int i = 0; i < strlen(plain); i++) { // Step 1: for every char in message
		*curr_char = *(plain+i);
		*curr_key_digit = *(key+(i%strlen(key)));
		char_ptr = caesar_decrypt(curr_char, curr_key_digit); // send next char and digit to decrypt
		*(plain + i) = *char_ptr; // put decrypted char in plain
		free(char_ptr);
	}

	free(curr_char);
	free(curr_key_digit);
		
	plain = caesar_decrypt(plain, key); // Step 2: run through normal caesar cipher
	
	return plain;
}

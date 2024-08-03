#include "caesar.h"
#include "augustus.h"
#include "aes_cbc.h"
#include "strings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>



void static print_string(string* s, string_type st);



/*
 * Creates new string and initializes string->plain, with or without 16-byte buffer.
 */
string *new_plain(char *s, int roundup) {
	
	string *new_string = malloc(sizeof(string));
	
	// either set up buffered plain memory
	if (roundup) {
		new_string->len = strlen(s) + (16 - (strlen(s)%16));
		new_string->plain = calloc(new_string->len, sizeof(char));
	// or normal plain memory
	} else {
		new_string->len = strlen(s);
		new_string->plain = calloc(new_string->len+1, sizeof(char));
	}
	// copy given message to plain
	strcpy(new_string->plain, s);
	
	// function pointers
	new_string->print = print_string;
	new_string->encrypt = encrypt_string;
	new_string->decrypt = decrypt_string;

	return new_string;
}



/*
 * Creates new string and initializes string->cipher, with or without 16-byte buffer
 */
string *new_cipher(char *s, int len, int roundup) {
	
	string *new_string = malloc(sizeof(string));
	
	// either set up buffered cipher memory
	if (roundup) {
		new_string->len = strlen(s) + (16 - (strlen(s)%16));
		new_string->cipher = calloc(new_string->len, sizeof(char));
	// or normal cipher memory
	} else {
		new_string->len = strlen(s);
		new_string->cipher = calloc(new_string->len+1, sizeof(char));
	}
	// copy given message to cipher
	strcpy(new_string->cipher, s);

	// initialize plain
	new_string->plain = NULL;
	
	// function pointers
	new_string->print = &print_string;
	new_string->encrypt = &encrypt_string;
	new_string->decrypt = &decrypt_string;
	
	return new_string;

}



/*
 * Creates and returns a new string.
 * Initializes string->plain with given string, and string->cipher using the specified cipher and key.
 */
string *encrypt_string(cipher c, char *s, char *key) {
	
	// set up the new string and load in plain
	string *new_string;
	if (c == AES) {
		new_string = new_plain(s, 1); // buffered plain
	} else {
		new_string = new_plain(s, 0); // non-buffered plain
	}
	
	// load in cipher
	if (c == CAESAR) { // if caesar
		new_string->cipher = calloc(new_string->len+1, sizeof(char)); // prep memory for cipher (+1 for '\0')
		strcpy(new_string->cipher, caesar_encrypt(s, key)); // copy encrypted plain to cipher

	} else if (c == AUGUSTUS) { // if augustus
		new_string->cipher = calloc(new_string->len+1, sizeof(char)); // prep memory for cipher (+1 for '\0')
		strcpy(new_string->cipher, augustus_encrypt(s, key)); // copy encrypted plain to cipher
	
	} else { // if AES
		
		struct AES_ctx *new_AES = malloc(sizeof(struct AES_ctx)); // create AES_ctx for encryption

		char *newiv = calloc(16, sizeof(uint8_t)); // prepare initialization vector
		setiv_string(newiv);
		
		AES_init_ctx_iv(new_AES, (uint8_t *)key, (uint8_t *)newiv); // initialize AES_ctx
		
		uint8_t *buf = malloc(new_string->len); // buffer to send plain, receive encrypted
		memcpy(buf, new_string->plain, new_string->len);
		
		AES_CBC_encrypt_buffer(new_AES, buf, new_string->len); // encrypt
		
		new_string->cipher = malloc(new_string->len); // put encryption in string->cipher
		memcpy(new_string->cipher, buf, new_string->len);

		free(new_AES); // free memory
		free(newiv);
		free(buf);

	}
	return new_string;

}



/*
 * Returns decrypted string->cipher using the specified cipher and key.
 * Copies decrypted message into string->plain.
 */
char *decrypt_string(cipher c, string *str, char *key) {
		
	// load in plain
	if (c == CAESAR) { // if caesar
		str->plain = calloc(str->len+1, sizeof(char)); // prep memory for plain (+1 for '\0')
		strcpy(str->plain, caesar_decrypt(str->cipher, key)); // copy decrypted cipher to plain

	} else if (c == AUGUSTUS) { // if augustus
		str->plain = calloc(str->len+1, sizeof(char)); // prep memory for plain (+1 for '\0')
		strcpy(str->plain, augustus_decrypt(str->cipher, key)); // copy decrypted cipher to plain
	
	} else { // if AES
		
		struct AES_ctx *new_AES = malloc(sizeof(struct AES_ctx)); // create AES_ctx for decryption
		
		char *newiv = calloc(16, sizeof(uint8_t)); // prepare initialization vector
		setiv_string(newiv);
		
		AES_init_ctx_iv(new_AES, (uint8_t *)key, (uint8_t *)newiv); // initialize AES_ctx
	
		uint8_t *buf = malloc(str->len); // buffer to send encrypted, receive plain
		memcpy(buf, str->cipher, str->len);

		AES_CBC_decrypt_buffer(new_AES, (uint8_t *)buf, str->len); // decrypt
		
		free(str->plain); // free put plain in string->plain
		str->plain = NULL;
		
		str->plain = malloc(str->len);
		memcpy(str->plain, buf, str->len);

		free(new_AES); // free memory
		free(newiv);
		return((char *)buf);

	}
	return str->plain;

}



/*
 * Copies default initialization vector into iv.
 */
void setiv_string(char *newiv) {
	uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	memcpy(newiv, iv, 16);
}



/*
 * Prints given string using hex, then plain text, in rows of 16 characters.
 */
void print_C_string(char *s) {
	
	int i; // columns
	int row; // rows
	
	//printf("%s\n", s);
	for (row = 0; row < strlen(s); row+=16) {
		for (i = row; i < row+16; i++)
			if (i >= strlen(s)) {
				printf("_0 ");
				continue;
			} else {
				printf("%.2x " , s[i] & 0xff);
			}
		printf(" | ");
		for (int i = row; i < row+16; i++)
			if (isprint(s[i]))
				printf("%c" , s[i]);
			else
				printf(" ");
		
		printf("\n");
	
	}
}



/*
 * Prints either string->plain or string->cipher.
 * First uses hex, then plain text, in rows of 16 characters.
 */
void static print_string(string *s, string_type st) {
	
	//plain or cipher
	char *text = s->plain;
	if (st == CIPHER) {
		text = s->cipher;
	}
	
	int i; // columns
	int row; // rows
	
	for (row = 0; row < s->len; row+=16) { // new rows until greater than string->len
		for (i = row; i < row+16; i++) // 16 columns per row
			
			// if no more chars, print "_0"
			if (i >= s->len) {
				printf("_0 ");
				continue;
			// else print char (hex)
			} else {
				printf("%.2x " , text[i] & 0xff); 
			}

		printf(" | ");

		for (int i = row; i < row+16; i++) // 16 more columns
			if (i >= s->len) {
				printf(" ");
			// if char is text, print
			} else if (isprint(*(text + i))) {
				printf("%c" , text[i]);
			// else print " "
			} else {
				printf(" ");
			}

		printf("\n");
	
	}
	
}

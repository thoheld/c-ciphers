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
	
	// function pointers
    new_string->print = &print_string;
    new_string->encrypt = &encrypt_string;
    new_string->decrypt = &decrypt_string;
    
    return new_string;

}



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
    
	} else { // if AES @@@@@@
        struct AES_ctx ctx;
        uint8_t in[(new_string->len)];
        uint8_t iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
        memcpy(in, s, new_string->len);
    
        AES_init_ctx_iv(&ctx, key, iv);
        AES_CBC_encrypt_buffer(&ctx, in, 16);
        
        memcpy(new_string->cipher, in, new_string->len);
    }
    return new_string;

}



char *decrypt_string(cipher c, string *str, char *key) {
    	
	// load in plain
    if (c == CAESAR) { // if caesar
    	str->plain = calloc(str->len+1, sizeof(char)); // prep memory for plain (+1 for '\0')
        strcpy(str->plain, caesar_decrypt(str->cipher, key)); // copy encrypted plain to plain

    } else if (c == AUGUSTUS) { // if augustus
    	str->plain = calloc(str->len+1, sizeof(char)); // prep memory for plain (+1 for '\0')
        strcpy(str->plain, augustus_decrypt(str->cipher, key)); // copy encrypted plain to plain
    
	} else { // if AES @@@@@@
        struct AES_ctx ctx;
        uint8_t in[(str->len)];
        uint8_t iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
        memcpy(in, str->cipher, str->len);
    
        AES_init_ctx_iv(&ctx, key, iv);
        AES_CBC_decrypt_buffer(&ctx, in, 16);
        
        memcpy(str->cipher, in, str->len);
    }
    return str->plain;

}



void setiv_string(char *newiv) {
    uint8_t temp[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
}



void print_C_string(char *s) {
    int i;
    int sec;
    printf("%s\n", s);
    for (sec = 0; sec < strlen(s); sec+=16) {
        for (i = sec; i < sec+16; i++)
            if (i >= strlen(s)) {
                printf("_0 ");
                continue;
            } else {
                printf("%.2x " , s[i] & 0xff);
            }
        printf(" | ");
        for (int i = sec; i < sec+16; i++)
            if (isprint(s[i]))
                printf("%c" , s[i]);
            else
                printf(" ");
        
        printf("\n");
    
    }
}



void static print_string(string *s, string_type st) {
    char *a = s->plain;

    if (st == CIPHER) {
        a = s->cipher;
    }
    
	int i;
    int sec;
    for (sec = 0; sec < s->len; sec+=16) {
        for (i = sec; i < sec+16; i++)
            if (i >= s->len) {
                printf("_0 ");
                continue;
            } else {
                printf("%.2x " , a[i] & 0xff);
            }
        printf(" | ");
        for (int i = sec; i < sec+16; i++)
            if (isprint(a[i]))
                printf("%c" , a[i]);
            else
                printf(" ");
        
        printf("\n");
    
    }
    
}

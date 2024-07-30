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
    string *a = malloc(sizeof(string));
    if (roundup) {
        a->len = strlen(s) + (16 - (strlen(s)%16));
        a->plain = malloc(a->len);
    } else {
        a->len = strlen(s);
        a->plain = malloc(a->len +1);
    }
    strcpy(a->plain, s);
    
    a->print = print_string;
    a->encrypt = encrypt_string;
    a->decrypt = decrypt_string;

    return a;
}

string *new_cipher(char *s, int len, int roundup) {
    string *a = malloc(sizeof(string));
    if (roundup) {
        a->len = strlen(s) + (16 - (strlen(s)%16));
        a->cipher = malloc(a->len);
    } else {
        a->len = strlen(s);
        a->cipher = malloc(a->len + 1);
    }
    strcpy(a->cipher, s);

    a->print = &print_string;
    a->encrypt = &encrypt_string;
    a->decrypt = &decrypt_string;
    
    return a;

}

string *encrypt_string(cipher c, char *s, char *key) {
    string *a;
    if (c == AES) {
        a = new_plain(s, 1);
    } else {
        a = new_plain(s, 0);
    }
    a->cipher = malloc(a->len+1);
    if (c == CAESAR) {
        strcpy(a->cipher, caesar_encrypt(s, key));
    } else if (c == AUGUSTUS) {
        strcpy(a->cipher, augustus_encrypt(s, key));
    } else {
        struct AES_ctx ctx;
        uint8_t in[(a->len)];
        uint8_t iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
        memcpy(in, s, a->len);
    
        AES_init_ctx_iv(&ctx, key, iv);
        AES_CBC_encrypt_buffer(&ctx, in, 16);
        
        memcpy(a->cipher, in, a->len);
    }
    return a;
}

char *decrypt_string(cipher c, string *str, char *key) {
    //char *a = malloc(strlen(str->cipher)+1);
    char *a = malloc(str->len + 1);

    if (c == CAESAR) {
        a = caesar_decrypt(str->cipher, key);
    } else if (c = AUGUSTUS) {
        a = augustus_decrypt(str->cipher, key);
    } else {
        struct AES_ctx ctx;
        uint8_t in[(str->len)];
        uint8_t iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
        memcpy(in, str->cipher, str->len);

        AES_init_ctx_iv(&ctx, key, iv);
        AES_CBC_decrypt_buffer(&ctx, in, 16);
        memcpy(a, in, str->len);
        
    }
    return a;
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

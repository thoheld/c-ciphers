#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "caesar.h"

char *augustus_encrypt(char *plain, char *key) {
    char *initEnc;
    initEnc = caesar_encrypt(plain, key);

    int keyPos = 0;
    char *finalEnc = malloc(sizeof(plain)+1);
    /*char **keyDigitChars = malloc(strlen(key));
    char **keyDigitInts = malloc(strlen(key));
    for (int i = 0; i < strlen(key); i++) {
        keyDigitChars[i] = &initEnc[i];
    }
    for (int i = 0; i < strlen(key); i++) {
        *keyDigitInts[i] = atoi(keyDigitChars[i]);
    }
    */
    char temp;
    for (int i = 0; i < strlen(plain); i++) {
        temp = key[keyPos];
        if (initEnc[i] == 32) {
            finalEnc[i] = initEnc[i];
        } else {
            finalEnc[i] = caesar_encrypt_char(initEnc[i], atoi(&temp));
        }
        if (keyPos == (strlen(key)-2)) {
            keyPos = 0;
            continue;
        } else {
            keyPos++;
        }
    }
    return finalEnc;
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



#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *caesar_encrypt(char *plain, char *key) {
    
    char *cipher = malloc(strlen(plain));
    
    char *p;;
    int add;
    //printf("%d\n", add);
    int i = 0;
    for (p = plain; *p != 0; p++) {
        add = atoi(key)%26;
        //printf("*p + add: %d, p: %d\n", *p + add, *p);
        
        if (*p == 32) {
            cipher[i] = 32;
        } else if (96 < *p && *p < 123) {
            while (*p + add > 122){
                add -= 26;
            }
            cipher[i] = (*p + add);
        } else if (64 < *p && *p < 91) {
            while (*p + add > 90){
                add -= 26;
            }
            cipher[i] = (*p + add);
        } else if (32 < *p && *p < 59) {
            while (*p + add > 58){
                add -= 26;
            }
            cipher[i] = (*p + add);
        }
        i++;
    }
    //printf("%s\n", cipher);
    return cipher;
}

char *caesar_decrypt(char *cipher, char *key) {
    
    char *plain = malloc(sizeof(plain));
    
    char *p;;
    int add;
    //printf("%d\n", add);
    int i = 0;
    for (p = cipher; *p != 0; p++) {
        add = 26 - (atoi(key)%26);
        //printf("*p + add: %d, p: %d\n", *p + add, *p);
        
        if (*p == 32) {
            plain[i] = 32;
        } else if (96 < *p && *p < 123) {
            while (*p + add > 122){
                add -= 26;
            }
            plain[i] = (*p + add);
        } else if (64 < *p && *p < 91) {
            while (*p + add > 90){
                add -= 26;
            }
            plain[i] = (*p + add);
        } else if (32 < *p && *p < 59) {
            while (*p + add > 58){
                add -= 26;
            }
            plain[i] = (*p + add);
            }
        i++;
    }
 

    return plain;
}

char caesar_encrypt_char(char plain, int key) {
    char cipher;
    int add = key%26;    
        if (96 < plain && plain < 123) {
            while (plain + add > 122){
                add -= 26;
            }
            cipher = (plain + add);
        } else if (64 < plain && plain < 91) {
            while (plain + add > 90){
                add -= 26;
            }
            cipher = (plain + add);
        } else if (32 < plain && plain < 59) {
            while (plain + add > 58){
                add -= 26;
            }
            cipher = (plain + add);
        }
        return cipher;

}

char caesar_decrypt_char(char cipher, int key) {
    char plain;
    int add = 26-(key%26);    
        if (96 < cipher && cipher < 123) {
            while (cipher + add > 122){
                add -= 26;
            }
            plain = (cipher + add);
        } else if (64 < cipher && cipher < 91) {
            while (cipher + add > 90){
                add -= 26;
            }
            plain = (cipher + add);
        } else if (32 < cipher && cipher < 59) {
            while (cipher + add > 58){
                add -= 26;
            }
            plain = (cipher + add);
        }
        return plain;

}

int convert_key(char *key) {
    int keyInt;
    int len = strlen(key);
    int hasLetters = 0;

    for (int i = 0; i<len; i++) {
        if (key[i] > 57 || key[i] < 48) {
            hasLetters = 1;
        }
    }

    if (hasLetters) {
        char *ans = malloc(sizeof(char));
        *ans = 0^key[0];
        for (int i = 1; i < len; i++) {
            *ans = *ans^key[i];
        }
        return (int) 0 | *ans;
    }

    return atoi(key);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "caesar.h"

char *caesar_encrypt(char *plain, char *key) {
	
	char *cipher = calloc(strlen(plain) + 1, sizeof(char));

	int i = 0;

	for (char *p = plain; *p != '\0'; p++) {	// for every char in plain
		cipher[i] = caesar_encrypt_char((char) *p, convert_key(key));
		i++;
	}
	
	return cipher;

}

char *caesar_decrypt(char *cipher, char *key) {

	char *plain = calloc(strlen(cipher) + 1, sizeof(char));

	int i = 0;

	for (char *c = cipher; *c != '\0'; c++) {
		plain[i] = caesar_decrypt_char((char) *c, convert_key(key));
		i++;
	}

	return plain;

}

char caesar_encrypt_char(char plain, int key) {
	
	char cipher;
	
	if (33 <= plain && plain <= 58) {	// from '!' to ':'
		cipher = plain - 33;	// remove offset
		cipher += key;	// apply key
		cipher %= 26;	// remove overflow
		cipher += 33; // add offset
		return cipher;

	} else if (65 <= plain && plain <= 90) {	// from 'A' to 'Z'
		cipher = plain - 65;	// remove offset
		cipher += key;	// apply key
		cipher %= 26;	// remove overflow
		cipher += 65; // add offset
		return cipher;

	} else if (97 <= plain && plain <= 122) {	// from 'a' to 'z'
		cipher = plain - 97;	// remove offset
		cipher += key;	// apply key
		cipher %= 26;	// remove overflow
		cipher += 97; // add offset
		return cipher;
	
	} else {	// all other characters
		return plain;
	}

}

char caesar_decrypt_char(char cipher, int key) {
	
	char plain;
	key = 26-(key%26);	// opposite of key, so we can add instead of subtract

	if (33 <= cipher && cipher <= 58) {	// from '!' to ':'
		plain = cipher - 33;	// remove offset
		plain += key;	// apply key
		plain %= 26;	// remove overflow
		plain  += 33; // add offset
		return plain;

	} else if (65 <= cipher && cipher <= 90) {	// from 'A' to 'Z'
		plain = cipher - 65;	// remove offset
		plain += key;	// apply key
		plain %= 26;	// remove overflow
		plain += 65; // add offset
		return plain;

	} else if (97 <= cipher && cipher <= 122) {	// from 'a' to 'z'
		plain = cipher - 97;	// remove offset
		plain += key;	// apply key
		plain %= 26;	// remove overflow
		plain += 97; // add offset
		return plain;
	
	} else {	// all other characters
		return cipher;
	}
	
	}

int convert_key(char *key) {
    int len = strlen(key);
    int hasLetters = 0;

    for (int i = 0; i<len; i++) {
        if (key[i] > 57 || key[i] < 48) {
            hasLetters = 1;
			break;
        }
    }

    if (hasLetters) {
        char *ans = malloc(sizeof(char));
        *ans = 0^key[0];
        for (int i = 1; i < len; i++) {
            *ans = *ans^key[i];
        }
        int final_ans = (int) 0 | *ans;	// so I can free ans
		free(ans);
		return final_ans;
    }

    return atoi(key);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "caesar.h"



/*
 * Encrypts a string using the caesar cipher.
 * Returns pointer to encrypted string.
 */
char *caesar_encrypt(char *plain, char *key) {
	
	char *cipher = calloc(strlen(plain) + 1, sizeof(char));

	// convert chars one by one
	int i = 0;
	for (char *p = plain; *p != '\0'; p++) {
		cipher[i] = caesar_encrypt_char((char) *p, convert_key(key)); // put converted char in cipher
		i++;
	}
	
	return cipher;

}



/*
 * Decrypts a string using the caesar cipher.
 * Retruns pointer to decrypted string.
 */
char *caesar_decrypt(char *cipher, char *key) {

	char *plain = calloc(strlen(cipher) + 1, sizeof(char));

	// convert chars one by one
	int i = 0;
	for (char *c = cipher; *c != '\0'; c++) {
		plain[i] = caesar_decrypt_char((char) *c, convert_key(key)); // put converted char in plain
		i++;
	}

	return plain;

}



/*
 * Encrypts sinle char using caesar cipher.
 * Returns encrypted char.
*/
char caesar_encrypt_char(char plain, int key) {
	
	char cipher;
	
	// chars from '!' to ':'
	if (33 <= plain && plain <= 58) {
		cipher = plain - 33; // remove offset
		cipher += key; // apply key
		cipher %= 26; // remove overflow
		cipher += 33; // add offset
		return cipher;
	
	// chars from 'A' to 'Z'
	} else if (65 <= plain && plain <= 90) {
		cipher = plain - 65; // remove offset
		cipher += key; // apply key
		cipher %= 26; // remove overflow
		cipher += 65; // add offset
		return cipher;

	// chars from 'a' to 'z'
	} else if (97 <= plain && plain <= 122) {
		cipher = plain - 97; // remove offset
		cipher += key; // apply key
		cipher %= 26; // remove overflow
		cipher += 97; // add offset
		return cipher;
	
	// all other chars
	} else {
		return plain;
	}

}



/*
 * Decrypts sinle char using caesar cipher.
 * Returns decrypted char.
*/
char caesar_decrypt_char(char cipher, int key) {
	
	char plain;
	key = 26-(key%26); // opposite of key, so we can add instead of subtract
	
	// chars from '!' to ':'
	if (33 <= cipher && cipher <= 58) {
		plain = cipher - 33; // remove offset
		plain += key; // apply key
		plain %= 26; // remove overflow
		plain  += 33; // add offset
		return plain;

	// chars from 'A' to 'Z'
	} else if (65 <= cipher && cipher <= 90) {
		plain = cipher - 65; // remove offset
		plain += key; // apply key
		plain %= 26; // remove overflow
		plain += 65; // add offset
		return plain;

	// chars from 'a' to 'z'
	} else if (97 <= cipher && cipher <= 122) {
		plain = cipher - 97; // remove offset
		plain += key; // apply key
		plain %= 26; // remove overflow
		plain += 97; // add offset
		return plain;
	
	// all other char
	} else {
		return cipher;
	}
	
}



/*
 * Converts key to int.
 */
int convert_key(char *key) {

	int len = strlen(key);
	int hasLetters = 0;
	
	// check if key has letters (if not, simple conversion to int)
	for (int i = 0; i<len; i++) {
		if (key[i] > 57 || key[i] < 48) {
			hasLetters = 1;
			break;
		}
	}

	// if key has letters, starting with 0, xor through chars to generate key
	if (hasLetters) {
		char *ans = malloc(sizeof(char));
		*ans = 0;
		for (int i = 0; i < len; i++) {
			*ans = *ans^key[i];
		}
		int final_ans = (int) 0 | *ans;	// so I can free ans
		free(ans);
		return final_ans;
	}

	return atoi(key); // simple conversion
}

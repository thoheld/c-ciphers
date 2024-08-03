#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "strings.h"
#include "aes_cbc.h"

void default_tests();
void file_input();
void segment_one_processor();
void segment_two_processor();
char *segment_three_processor();
void AES_key_filter();
void print_handler();
char *str_to_hex();

int main(int argc, char *argv[]) { 
	
	cipher cipher_type = CAESAR;
	
	if (argc > 1) {

		if (strcmp(argv[1], "default") == 0) {
			default_tests();
			exit(1);
		
		} else if (strcmp(argv[1], "ciphers.txt") == 0) {
			file_input(argv[1]);
		}
	
	}
			
	printf("\nDefault Keys:\n");
	printf("Caesar: 1\n");
	printf("Augustus: 12\n");
	printf("AES: 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6\n     0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c\n");
	printf("\n");

	int running = 1;
	while (running) {   
			char in_c[3];
			int gettingCMD = 1;
			while (gettingCMD) {
				printf("Enter command - q(uit), c(aesar), au(gustus), ae(s): ");
				fgets(in_c, 3, stdin);
				if (strcmp(in_c, "c\n") == 0) {
					cipher_type = CAESAR;
					break;
				} else if (strcmp(in_c, "au") == 0) {
					cipher_type = AUGUSTUS;
					break;
				} else if (strcmp(in_c, "ae") == 0) {
					cipher_type = AES;
					break;
				} else if (strcmp(in_c, "q\n") == 0) {
					exit(1);
				} else {
					printf("Please enter a command!\n");
				}
			}
			printf("\n");
			
			int catchFirst = 0;
			if (cipher_type == AES || cipher_type == AUGUSTUS) {
				catchFirst = 1;
			}
			char in_s[120];
			int waitingForString = 1;
			//fgets(in_s, 120, stdin);
			while (waitingForString) {
				if (!catchFirst) {
					printf("Enter string: ");
				}
				fgets(in_s, 120, stdin);
				in_s[strcspn(in_s, "\n")] = '\0';
				printf("\n");

				if (catchFirst && strlen(in_s) == 0) {
					catchFirst = 0; // fixes skipping problem when au or ae are chosen
					continue;
				} else if (strlen(in_s) == 0) {
					printf("You must enter a string!\n");
					continue;
				} else {
					break;
				}
			}

			char *in_k = malloc(120);
			printf("Enter key (Enter for default): ");
			fgets(in_k, 120, stdin);
			in_s[strcspn(in_s, "\n")] = '\0';  // replace \n with \0
			if (strcmp(in_k, "\n") == 0) {
				if (cipher_type == CAESAR) {
					in_k[0] = '1';
					printf("Using default key.\n");
				} else if (cipher_type == AUGUSTUS) {
					for (int i = 0; i < 200; i++) {
						in_k[i] = '\0';
					}
					in_k[0] = '1';
					in_k[1] = '2';
					in_k[2] = '\0';
					printf("Using default key.\n");
				} else if (cipher_type == AES) {
					char def_aes_k[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
					strcpy(in_k, def_aes_k);
					printf("Using default key.\n");
				}
			}
			
			string *ms;
			if (cipher_type == AES) {
				ms = new_plain(in_s, 1);
			} else {
				ms = new_plain(in_s, 0);
			}
			
			
			printf("\nPlain text string:\nlen: %d\n", ms->len);
			ms->print(ms, PLAIN);

			string *b = ms->encrypt(cipher_type, in_s, in_k);
			printf("\nCipher text string:\nlen: %d\n", ms->len);
			b->print(b, CIPHER);

			char *s = b->decrypt(cipher_type, b, in_k);
			printf("\nDecrypted input:\n");
			printf("%s\n", s);
			printf("len: %lu\n", strlen(s));
			
			print_C_string(s);
			printf("\nlen: %lu\n", strlen(s));
			b->print(b, PLAIN);
			printf("\n\n\n");

			free(ms->plain);
			free(ms->cipher);
			free(ms);
			free(b->plain);
			free(b->cipher);
			free(b);

		}
	return 0;
	}



/*
 * File handler
 */
void file_input(char *file_name) {
	
	FILE *file = fopen(file_name, "r");
	char line[200]; // store line
	int triplet_segment = 1; // 1, 2, or 3
	int ready_to_print = 0; // 0 or 1
	int *encrypt_or_decrypt = malloc(sizeof(int)); // 1 for encrypt, 2 for decrypt
	cipher *encryption_method = malloc(sizeof(cipher));
	char *key = NULL;
	char *message = NULL;
	int triplet_number = 1;

	while (fgets(line, 200, file)) { // next line in file
		
		//printf("%s\n", line);	
		
		if (line[0] == '#') continue; // skip # comments

		line[strlen(line)-1] = '\0'; // replace '\n' with '\0'
		
		// first segment (encrypt/decrypt and cipher)
		if (triplet_segment == 1) {
			segment_one_processor(line, encrypt_or_decrypt, encryption_method);
			triplet_segment = 2; // next segment
		
		// second segment (key)
		} else if (triplet_segment == 2) { // second segment
			segment_two_processor(line, &key, *encryption_method);
			triplet_segment = 3; // next segment

		// third segment (message)
		} else if (triplet_segment == 3) { // last segment
			
			message = segment_three_processor(*encrypt_or_decrypt, *encryption_method, line);
			ready_to_print = 1; // all 3 segments processed, ready to print
			triplet_segment = 1; // back to first segment

		}

		if (ready_to_print) {
			print_handler(triplet_number++, *encrypt_or_decrypt, encryption_method, key, message);
			if (message != NULL) {
				free(message);
				message = NULL;
			}
			ready_to_print = 0;
		}

	}

	exit(1);

}


/*
 * Process first segment of triplet.
*/
void segment_one_processor(char *line, int *encrypt_or_decrypt, cipher *encryption_method) {
	
	char *str_encrypt_or_decrypt =  strtok(line, " ");
	if (strcmp(str_encrypt_or_decrypt, "encrypt") == 0) {
		*encrypt_or_decrypt = 1;
	} else {
		*encrypt_or_decrypt = 2;
	}

	char *str_encryption_method =  strtok(NULL, " ");
	if (strcmp(str_encryption_method, "caesar") == 0) {
		*encryption_method = CAESAR;
	} else if (strcmp(str_encryption_method, "augustus") == 0) {
		*encryption_method = AUGUSTUS;
	} else {
		*encryption_method = AES;
	}

}



/*
 * Process second segment of triplet.
*/
void segment_two_processor(char *line, char **key, cipher encryption_method) {
	
	if (*key != NULL) {
		free(*key);
		*key = NULL;
	}
	
	// default key
	if(strcmp(line, "default") == 0) {
		if (encryption_method == CAESAR) {
			*key = malloc(2);
			strcpy(*key, "1");
		
		} else if (encryption_method == AUGUSTUS) {
			*key = malloc(3);
			strcpy(*key, "12");
		
		} else {
			*key = malloc(16);
			char temp_key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
			memcpy(*key, temp_key, 16);
		}
		return;
	}
	
	// make sure AES key is proper
	if (encryption_method == AES) {
		AES_key_filter(line, key); // key here is double pointer
		return;
	}

	// caesar and augustus keys
	*key = malloc(strlen(line) + 1);
	strcpy(*key, line);

}



/*
 * Ensures key is 16 bytes.
 */
void AES_key_filter(char *line, char **key) {
	
	*key =  str_to_hex(line, 47);
	
	// if 16 or more chars given, use first 16 chars for key
	if (strlen(line) >= 16) {
		memcpy(*key, line, 16);
		return;
	}
	
	// add all given chars to key
	int index = 0;
	for (; index < strlen(line); index++) {
		memcpy((*key + index), (line + index), 1);
	}
	// and fill remaining slots with '0'
	for (; index < 16; index++) {
		*(*key + index) = '0';
	}	

}



/*
 * Constructs string and prints
 */
void print_handler(int triplet_number, int encrypt_or_decrypt, cipher *encryption_method, char *key, char *message) {
	
	string *new_string;

	// encrypt
	if (encrypt_or_decrypt == 1) {
		new_string = encrypt_string(*encryption_method, message, key);
	} else {
		// decrypt
		if (*encryption_method == AES) { // roundup = 1 for AES
			new_string = new_cipher(message, strlen(message), 1);
		} else {
			new_string = new_cipher(message, strlen(message), 0);
		}
		decrypt_string(*encryption_method, new_string, key);
	}

	//print
	if (encrypt_or_decrypt == 1) {
		switch (*encryption_method) {
			case CAESAR:
				printf("\nTriplet: %d, Cipher: Caesar, Encrypt, Length: %d\n", triplet_number, new_string->len);
				break;
			case AUGUSTUS:
				printf("\nTriplet: %d, Cipher: Augustus, Encrypt, Length: %d\n", triplet_number, new_string->len);
				break;
			case AES:
				printf("\nTriplet: %d, Cipher: AES, Encrypt, Length: %d\n", triplet_number, new_string->len);
				break;
		}
		new_string->print(new_string, CIPHER);

	} else {
		switch (*encryption_method) {
			case CAESAR:
				printf("\nTriplet: %d, Cipher: Caesar, Decrypt, Length: %d\n", triplet_number, new_string->len);
				break;
			case AUGUSTUS:
				printf("\nTriplet: %d, Cipher: Augustus, Decrypt, Length: %d\n", triplet_number, new_string->len);
				break;
			case AES:
				printf("\nTriplet: %d, Cipher: AES, Decrypt, Length: %d\n", triplet_number, new_string->len);
				break;
		}
		new_string->print(new_string, PLAIN);

	}

	free(new_string);
	return;

}



/*
 * Converts string of written hex values to chars containing actual hex value.
 */
char *str_to_hex(char *str, int len) {
	
	char *converted = malloc((len/3) + 1);

	char curr_hex_value;
	char curr_char;
	int low_or_high = 0; // high = 0, low = 1
	char high_digit = 0;
	char low_digit = 0;
	int converted_index = 0;
	for (int index = 0; index < len; index++) {
		
		curr_char = *(str + index);
		
		if (curr_char  == ' ') { continue; }
		
		if (low_or_high == 0) { // high_digit

			// letters
			if (curr_char >= 'a' && curr_char <= 'f') {
				high_digit = (curr_char - 'a') + 10; // convert to hex
			// numbers
			} else {
				high_digit = curr_char - '0';
			}
			high_digit = high_digit << 4; // multiply by 16

			low_or_high = 1; // prep for low
			
			continue;
		}

		if (low_or_high == 1) { // low_digit
			
			// letters
			if (curr_char >= 'a' && curr_char <= 'f') {
				low_digit = (curr_char - 'a') + 10; // convert to hex
			// numbers
			} else {
				low_digit = curr_char - '0';
			}
			
			// add digits together, store in key
			curr_hex_value = high_digit | low_digit; // add
			*(converted + converted_index) = curr_hex_value;
			
			high_digit = 0; // reset digits
			low_digit = 0;

			converted_index++;
			low_or_high = 0; // back to high

			continue;
		}
		
	}

	return converted;
		
}



char *segment_three_processor(int encrypt_or_decrypt, cipher encryption_method, char* line) {
	
	char *message;
	
	if (encryption_method == AES && encrypt_or_decrypt == 2) {
		message = str_to_hex(line, strlen(line));	
		return message;
	}
	
	message = malloc(strlen(line) + 1);
	strcpy(message, line);
	return message;
}

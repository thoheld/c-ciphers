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
void AES_key_filter();
void print_handler();

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
			
		if (ready_to_print) {
			print_handler(triplet_number++, encrypt_or_decrypt, encryption_method, key, message);
			if (message != NULL) {
				free(message);
				message = NULL;
			}
			ready_to_print = 0;
		}
		
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
			
			message = malloc(strlen(line) + 1);
			strcpy(message, line);
			ready_to_print = 1; // all 3 segments processed, ready to print
			triplet_segment = 1; // back to first segment

		}

	}

	exit(1);

	/*FILE *file = fopen("ciphers.txt", "r");
	char line[200];
	int tripNum = 1;
	int tripSeg = 1;
	char *p1;
	char *p2;
	int op;
	char ops[10];
	cipher c;
	char cs[10];
	char k[120];
	char s[200];
	string *p;

	while (fgets(line, 200, file)){
		if (tripSeg == 4) { // ready to print
			if (op == 1) {
				strcpy(ops, "Encrypt");
			} else {
				strcpy(ops, "Decrypt");
			}

			if (c == CAESAR) {
				strcpy(cs, "Caesar"); 
			} else if (c == AUGUSTUS) {
				strcpy(cs, "Augustus"); 
			} else {
				strcpy(cs, "AES"); 
			}

			printf("Triplet %d: Cipher: %s, %s len: %lu\n", tripNum, cs, ops, strlen(s));
		   
			if (op == 1) {
				p = encrypt_string(c, s, k);
				print_C_string(p->cipher);
				free(p->plain);
				free(p->cipher);
				free(p);
			}  else if (op == 2) {
				if (c == AES) {
					p = new_cipher(s, strlen(s), 1);
					print_C_string(p->decrypt(c, p, k));
					free(p->cipher);
					free(p);
				} else {
					p = new_cipher(s, strlen(s), 0);
					print_C_string(p->decrypt(c, p, k));
					free(p->cipher);
					free(p);
				}
			}

			printf("\n");
			tripSeg = 1;
			tripNum++;
		}

		if (line[0] == '#') continue; // skip # comments
	   

		if (tripSeg == 1) { // reading cipher and operation
			p1 =  strtok(line, " ");
			p2 =  strtok(NULL, " ");
			if (strcmp(p1, "encrypt") == 0) {
				op = 1;
			} else {
				op = 2;
			}

			if (strcmp(p2, "caesar\n") == 0) {
				c = CAESAR;
			} else if(strcmp(p2, "augustus\n") == 0) {
				c = AUGUSTUS;
			} else {
				c = AES;
			} 
			tripSeg = 2;


		} else if (tripSeg == 2) { // reading key
		strcpy(k, calloc(120, sizeof(char))); // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ changing this changed it
			if(strcmp(line, "default\n") == 0) {
				if (c == CAESAR) {
					k[0] = '1';
				} else if (c == AUGUSTUS) {
					strcpy(k, "12"); // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ this doesnt work :(
				} else {
					uint8_t def_aes_k[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
					strcpy(k, def_aes_k);
				}
			} else {
				line[strcspn(line, "\n")] = '\0';  // replace \n with \0
				strcpy(k, line);
				if (c == AES) { // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ i have no idea what to do here
					//char actual_key[16];
					//char val = 0;
					//for (int i = 0; i < 48; i+=3) {
						//actual_key[i/3] = atoi()
					//}
					uint8_t def_aes_k[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
					strcpy(k, def_aes_k);

				}
			}
			tripSeg = 3;


		} else if (tripSeg == 3){ // reading string
			strcpy(s, line);
			s[strcspn(s, "\n")] = '\0';  // replace \n with \0
			tripSeg = 4;
		}
	}

	fclose(file);
	exit(1);*/
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
		printf("here");
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
	if(strcmp(line, "default\n") == 0) {
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
	
	*key = malloc(16);
	//printf("%lu", strlen(line));
	// if 16 or more chars given, use first 16 chars for key
	if (strlen(*key) >= 16) {
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
void print_handler(int triplet_number, int *encrypt_or_decrypt, cipher *encryption_method, char *key, char *message) {
	
	string *new_string;

	// encrypt
	if (encrypt_or_decrypt) {
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
	if (encrypt_or_decrypt) {
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

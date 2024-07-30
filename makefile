default_tests: aes_cbc.c augustus.c caesar.c default_tests.c strings.c aes_cbc.c
	gcc -o go aes_cbc.c augustus.c caesar.c default_tests.c strings.c -I.

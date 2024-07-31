strings_tester: aes_cbc.c augustus.c caesar.c default_tests.c strings.c aes_cbc.c
	gcc -g -o go aes_cbc.c augustus.c caesar.c default_tests.c strings.c strings_tester.c -I.

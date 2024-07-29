string_tester: aes_cbc.c augustus.c caesar.c default_tests.c strings.c strings_tester.c
	gcc -o strings_tester aes_cbc.c augustus.c caesar.c default_tests.c strings.c strings_tester.c -I.


sha2-test: sha2-no-constants.c
	gcc -O2 -o $@ $^

sha2-test-min: sha2-no-constants-min.c
	gcc -Wall -Wpedantic -O3 -o $@ $^

sha2-test-release: sha2-no-constants.c
	gcc -s -O2 -o $@ $^

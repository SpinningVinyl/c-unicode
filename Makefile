test: test.c unicode.h unicode.c
	gcc -g -o test test.c unicode.c

clean:
	rm test

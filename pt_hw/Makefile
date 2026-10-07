CC = gcc
CFLAGS = -Wall -Wextra -std=c11

test_translate: mlpt.c test_translate.c
	$(CC) $(CFLAGS) -o test_translate mlpt.c test_translate.c

clean:
	rm -f test_translate

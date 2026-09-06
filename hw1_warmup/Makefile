CC = gcc
CFLAGS = -D_XOPEN_SOURCE=700 -Og -g -std=c11 -Wall -pedantic
ADDR = -fsanitize=address

all: split

split.o: split.h split.c
	$(CC) -c split.c -o split.o $(CFLAGS) $(ADDR)

main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS) $(ADDR)

split: main.o split.o
	$(CC) -o split main.o split.o $(CFLAGS) $(ADDR)

clean:
	rm -f *.o split

.PHONY: all clean
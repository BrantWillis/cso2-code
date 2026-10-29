CC = gcc
#CFLAGS = -Wall -Wextra -std=c11

all: tlb

tlb.o: tlb.c tlb.h config.h
	$(CC) -c tlb.c -o tlb.o

#mlpt.o: mlpt.c mlpt.h config.h
#	$(CC) $(CFLAGS) -c mlpt.c -o mlpt.o

tlb: tlb.o# mlpt.o
	$(CC) -o tlb tlb.o

clean:
	rm -f tlb.o tlb

.PHONY: all clean
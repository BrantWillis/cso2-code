CC = gcc
CFLAGS = -O1

all: gettimings

gettimings.o: gettimings.c
	$(CC) -c gettimings.c -o gettimings.o

gettimings: gettimings.o
	$(CC) $(CFLAGS) -o gettimings gettimings.c

clean:
	rm -f *.o gettimings

.PHONY: clean all
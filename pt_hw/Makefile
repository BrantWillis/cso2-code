CC = gcc
CFLAGS = -Wall -Wextra -std=c11
AR = ar
ARFLAGS = rcs

all: libmlpt.a

libmlpt.a: mlpt.o
	$(AR) $(ARFLAGS) libmlpt.a mlpt.o

mlpt.o: mlpt.c mlpt.h config.h
	$(CC) $(CFLAGS) -c mlpt.c -o mlpt.o

clean:
	rm -f mlpt.o libmlpt.a

.PHONY: all clean
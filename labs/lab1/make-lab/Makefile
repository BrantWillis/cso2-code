#worked with An Tiet (bqd5da)

CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LDFLAGS = -L. -Wl,-rpath .

all: guesser

cheer.o: cheer.c say.h
	$(CC) $(CFLAGS) -c cheer.c -o cheer.o

grunt.o: grunt.c say.h
	$(CC) $(CFLAGS) -c grunt.c -o grunt.o

libsay.so: cheer.o grunt.o
	$(CC) $(LDFLAGS) -shared cheer.o grunt.o -o libsay.so

guesser.o: guesser.c say.h
	$(CC) $(CFLAGS) -c guesser.c -o guesser.o

guesser: guesser.c | libsay.so
	$(CC) $(LDFLAGS) -o guesser libsay.so guesser.c -lsay

clean:
	rm -f *.o *.so guesser

.PHONY: all clean

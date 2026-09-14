CC = gcc

my_system.o: my_system.c my_system.h
	$(CC) -c my_system.c -o my_system.o

main.o: main.c my_system.h
	$(CC) -c main.c -o main.o

main: main.o my_system.o
	$(CC) -o $@ $^

clean:
	rm -f *.o main

.PHONY: all clean
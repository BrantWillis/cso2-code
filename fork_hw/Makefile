CC = gcc
CFLAGS = -fsanitize=address -ftrivial-auto-var-init=pattern -Og -g

all: test

fork_run.o: fork_run.c fork_run.h
	$(CC) -o fork_run.o -c fork_run.c

test.o: test.c fork_run.h
	$(CC) -o test.o -c test.c

test: fork_run.o test.o
	$(CC) -o test fork_run.o test.o $(CFLAGS)

clean:
	rm -f *.o *.txt test

.PHONY: all clean
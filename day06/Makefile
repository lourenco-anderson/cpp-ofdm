# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Wextra

all: matrix_struct test_matrix

matrix_struct: matrix.o main.o
	$(CC) $(CFLAGS) -o $@ $^

test_matrix: matrix.o test_matrix.o 
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o matrix_struct test_matrix
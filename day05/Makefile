# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Wextra

all: app testMySignal

app: mySignal.o main.o
	$(CC) $(CFLAGS) -o $@ $^

testMySignal: mySignal.o testSignal.o 
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o app testMySignal
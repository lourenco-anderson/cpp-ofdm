# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Wextra

# Source files and object files
SRCS = mySignal.c main.c
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = mySignal

# Default target
all: $(TARGET)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# mySignal: mySignal.c main.c
# 	gcc mySignal.c main.c -o mySignal -Wall -Wextra
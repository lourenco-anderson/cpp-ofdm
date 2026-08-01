#include <stdio.h>
#include <stdlib.h>

#ifndef MY_SIGNAL_H
#define MY_SIGNAL_H

struct Signal {
    double *data; // Pointer to dynamically allocated array of doubles
    size_t length; // Length of the array
};

struct Signal create_signal(size_t length);
void print_signal(struct Signal signal);
void free_signal(struct Signal signal);

#endif
#include <stdio.h>
#include <stdlib.h>

#ifndef MATRIX_H
#define MATRIX_H

struct Matrix {
    double *data; // Pointer to dynamically allocated array of doubles
    size_t rows; // Number of rows in the matrix
    size_t cols; // Number of columns in the matrix
};

struct Matrix create_matrix(size_t rows, size_t cols);
double get_element(struct Matrix matrix, size_t i, size_t j);
void set_element(struct Matrix matrix, size_t i, size_t j, double value);
void print_matrix(struct Matrix matrix);
void free_matrix(struct Matrix matrix);
#endif
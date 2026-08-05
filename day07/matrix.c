#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

struct Matrix create_matrix(size_t rows, size_t cols){
    if (rows == 0 || cols == 0) {
        printf("Invalid number of rows or columns. Returning Sentinel.\n");
        struct Matrix matrix;
        matrix.data = NULL;
        matrix.rows = 0;
        matrix.cols = 0;
        return matrix; 
    }
    
    struct Matrix matrix;
    double *data = malloc(rows * cols * sizeof(double)); 

    if (data == NULL) {
        printf("Memory allocation failed. Sentinel.\n");
        matrix.data = NULL;
        matrix.rows = 0;
        matrix.cols = 0;
        return matrix; 
    }

    for (size_t i = 0; i < rows; i++){
        for (size_t j = 0; j < cols; j++){
            data[i * cols + j] = i * cols + j; 
        }
    }
    matrix.rows = rows;
    matrix.cols = cols;
    matrix.data = data;

    return matrix;
}

double get_element(struct Matrix matrix, size_t i, size_t j){
    return matrix.data[i * matrix.cols + j];
}

void set_element(struct Matrix matrix, size_t i, size_t j, double value){
    matrix.data[i * matrix.cols + j] = value; 
}

struct Signal get_row(struct Matrix matrix, size_t i){
    // This function returns only a view of the pointer inside matrix. The output should not be used with free_signal
    struct Signal output;
    if (i >= matrix.rows){
        output.data = NULL;
        output.length = 0;
        return output;
    }

    output.length = matrix.cols;
    output.data = matrix.data + i * matrix.cols;
    
    return output;
}

void print_matrix(struct Matrix matrix){
    for(size_t i = 0; i <matrix.rows; i++){
        for(size_t j = 0; j < matrix.cols; j++){
            printf("%f ", get_element(matrix, i, j));
        }
        printf("\n");
    }
}

void print_signal(struct Signal signal) {
    for (size_t i = 0; i < signal.length; i++) {
        printf("Value at signal.data[%zu]: %f\n", i, signal.data[i]);
    }
}

void free_matrix(struct Matrix matrix){
    free(matrix.data);
}

void free_signal(struct Signal signal){
    free(signal.data);
}
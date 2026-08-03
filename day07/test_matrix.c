#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "matrix.h"

void test_row0 (void){
    size_t rows = 0;
    size_t cols = 5;

    struct Matrix matrix = create_matrix(rows, cols);
    assert(matrix.rows == rows);
    assert(matrix.cols == 0);
    assert(matrix.data == NULL);
}

void test_column0 (void){
    size_t rows = 5;
    size_t cols = 0;

    struct Matrix matrix = create_matrix(rows, cols);
    assert(matrix.rows == 0);
    assert(matrix.cols == cols);
    assert(matrix.data == NULL);
}

void test_get_element(void){
    size_t rows = 4;
    size_t cols = 4;

    struct Matrix matrix = create_matrix(rows, cols);
    for (size_t i = 0; i < rows; i++){
        for (size_t j = 0; j < cols; j++){
            assert(get_element(matrix, i , j) == i * cols + j);
        }
    }
    free_matrix(matrix);
}

void test_get_row(void){
    size_t rows = 4;
    size_t cols = 4;

    struct Matrix matrix = create_matrix(rows, cols);
    struct Signal row = get_row(matrix, 2);
    assert(row.data != NULL);
    assert(row.length == matrix.cols);
    for (size_t j = 0; j < row.length; j++) {
        assert(row.data[j] == get_element(matrix, 2, j));
    }
    struct Signal invalid_row = get_row(matrix, matrix.rows); // out of bounds
    assert(invalid_row.data == NULL);
    assert(invalid_row.length == 0);

    free_matrix(matrix);
}

int main() {
    test_row0();
    test_column0();
    test_get_element();
    test_get_row();
    return 0;
}
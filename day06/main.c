#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"


int main() {
    size_t rows, cols;
    printf("Enter the number of rows: ");
    scanf("%zu", &rows);
    printf("Enter the number of columns: ");
    scanf("%zu", &cols);


    struct Matrix matrix = create_matrix(rows, cols);
    if (matrix.data == NULL) {
        printf("Matrix creation failed. Exiting.\n");
        return 1; 
    }

    print_matrix(matrix);
    free_matrix(matrix);

    return 0;
}
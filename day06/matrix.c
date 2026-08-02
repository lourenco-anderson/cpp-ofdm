#include <stdio.h>
#include <stdlib.h>

void print_matrix(double *data, size_t rows, size_t cols){
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<cols; j++){
            printf("%f ", data[i * cols + j]);
        }
        printf("\n");
    }
}
int main() {
    size_t rows, cols;
    printf("Enter the number of rows: ");
    scanf("%zu", &rows);
    printf("Enter the number of columns: ");
    scanf("%zu", &cols);

    double *data = malloc(rows * cols * sizeof(double));
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            data[i * cols + j] = i*cols + j;
        }
    }

    print_matrix(data, rows, cols);
    free(data);
    return 0;
}
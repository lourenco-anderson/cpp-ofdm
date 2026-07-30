#include <stdio.h>

void print_matrix(double matrix[][8], size_t rows, size_t cols){
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<cols; j++){
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(){
    double matrix[4][8];

    size_t rows = sizeof(matrix) / sizeof(matrix[0]);
    size_t cols = sizeof(matrix[0]) / sizeof(matrix[0][0]);
    printf("Elements at main: \n");
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<cols; j++){
            matrix[i][j] = i*cols + j;
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("Elements at function: \n");
    print_matrix(matrix, rows, cols);

    printf("Fim da linha 0: %p\n", (void*)&matrix[0][7]);
    printf("Início da linha 1: %p\n", (void*)&matrix[1][0]);
    return 0;
}
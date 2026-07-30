#include <stdio.h>

void sum_avg(double *arr, int size, double *avg, double *sum){
    *sum = 0;
    for(int i = 0; i < size; i++){
        *sum += arr[i];
    }
    *avg = *sum / size;
} 

int main(){
    double arr[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    double avg = 0;
    double sum = 0; 
    
    sum_avg(arr, 8, &avg, &sum);
    printf("Sum: %f\n", sum);
    printf("Avg: %f\n", avg);
} 

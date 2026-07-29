#include <stdio.h>

double sum_avg(double *arr, int size, double *avg){
    double sum = 0;
    for(int i = 0; i < size; i++){
        sum += arr[i];
    }
    *avg = sum / size;
    return sum;
} 

int main(){
    double arr[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    double avg = 0;
    double sum = sum_avg(arr, 8, &avg);
    printf("Sum: %f\n", sum);
    printf("Average: %f\n", avg);
    return 0;
}
#include <stdio.h>

int main(){
    double arr[6] = {1, 2, 3, 4, 5, 6};
    double *ptr = arr; // Pointer to the first element of the array

    for(int i = 0; i<6; i++){
        printf("Address of arr[%d]: %p, Value: %f\n", i, (void*)&arr[i], *(ptr + i));
    }

    return 0;
}
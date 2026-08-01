#include <stdio.h>
#include <stdlib.h>

int main(){
    int num;
    printf("Give me a number: \n");   
    scanf("%d", &num);

    double *arr = malloc(num * sizeof(double)); // Dynamically allocate memory for an array of doubles
    if (num <= 0) {
        printf("Invalid number. Exiting.\n");
        return 1; // Exit if the number is not positive
    }
    if (arr == NULL) {
        printf("Memory allocation failed. Exiting.\n");
        return 1; // Exit if memory allocation fails
    }

    for(int i = 0; i < num; i++){
        arr[i] = i * 5; // Assign a value to the allocated memory

        printf("Value at arr[%d]: %f\n", i, arr[i]);
        printf("Address of arr[%d]: %p\n", i, (void*)&arr[i]); // Print the address of the allocated memory
    }
    
    free(arr); // Free the allocated memory for the array of pointers
    return 0;
}
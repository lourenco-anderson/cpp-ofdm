#include <stdio.h>
#include <stdlib.h>

struct Signal {
    double *data; // Pointer to dynamically allocated array of doubles
    size_t length; // Length of the array
};

struct Signal create_signal(size_t length) {
    struct Signal output;
    
    if (length == 0){
        printf("Invalid number. Exiting.\n");
        output.data = NULL;
        output.length = 0;
        return output; // Exit if the number is not positive; 
    }

    output.length = length;
    output.data =  malloc(length * sizeof(double));
    
    
    if (output.data == NULL){
        printf("Memory allocation failed. Exiting.\n"); 
        output.length = 0;
        return output; // Exit if memory allocation fails
    }

    for (size_t i = 0; i < length; i++){
        if (i%2 == 0){
            output.data[i] = 1;
        }else{
            output.data[i] = -1;
        }
    }
    return output;
}

void print_signal(struct Signal signal) {
    for (size_t i = 0; i < signal.length; i++) {
        printf("Value at signal.data[%zu]: %f\n", i, signal.data[i]);
    }
}

void free_signal(struct Signal signal){
    free(signal.data);
}

int main() {
    size_t num;
    printf("Give me a number: \n");   
    scanf("%zu", &num);

    struct Signal signal = create_signal(num);

    if (signal.data == NULL) {
        printf("...\n");
        return 1;
    }

    print_signal(signal);

    // Don't forget to free the allocated memory
    free_signal(signal);

    return 0;
}
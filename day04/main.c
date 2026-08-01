#include <stdio.h>
#include <stdlib.h>

#include "mySignal.h"

// This is a test comment for make
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
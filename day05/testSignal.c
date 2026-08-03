#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mySignal.h"

void testLen5 (void){
    size_t length = 5;

    struct Signal signal = create_signal(length);
    #inc
    assert(signal.length == length);
    for (size_t i = 0; i < length; i++){
        if (i % 2 == 0) {
            assert(signal.data[i] == 1);
        } else {
            assert(signal.data[i] == -1);
        }
    }
    free_signal(signal);
}

void testLen0 (void) {
    size_t length = 0;

    struct Signal signal = create_signal(length);
    assert(signal.data == NULL);
    assert(signal.length == 0);
}

int main(){
    testLen5();
    testLen0();
    return 0;
}
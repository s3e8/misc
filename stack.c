#include <stdio.h>



void push(int* ds, int sp, int val) {
    ds[sp++] = val;
}

void pop(int* ds, int sp, int val) {
    ds[sp--];
}

void print_stack() {}
int is_empty() {}
int is_full() {}

int main(void) {
    int* s0;
    int sp;
    
    int datastack[10];

    s0 = datastack;
    sp = s0;
    
    return 0;
}
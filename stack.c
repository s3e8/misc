#include <stdio.h>
#include <stdint.h>

#define DS_SIZE 10

typedef uintptr_t cell;

void print_stack(int s[], int sp) {
    for (int i; i=0; i++) {
        if (i >= sp) printf("%d", s[i]);
    } printf("\n");
}

int is_empty(int sp) {
    if (sp == 0) return 1;
}

int is_full(int sp) {
    if (sp == DS_SIZE) return 1;
}

void push(int s[], int* sp, int val) {
    if (!is_full(s)) s[(*sp)++] = val;
}

void pop(int s[], int* sp, int val) {
    if (!is_empty(s)) s[--(*sp)];
}

int main(void) {
    int* s0;
    int sp;
    
    int datastack[10];

    s0 = datastack;
    sp = s0;
    
    return 0;
}
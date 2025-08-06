#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define DS_SIZE 10
#define INBUF_SIZE 256
#define MAX_WORDS 16

typedef uintptr_t cell;

int* sp;
int s0[10];

void print_stack() {
    printf("ds: [ ");
    for (int* i = sp; i < s0 + DS_SIZE; i++) printf("%d ", *i);
    printf("]\n");
}

int is_empty() {
    return sp == s0 + DS_SIZE;
}

int is_full() {
    return sp == s0;
}

void push(int val) {
    if (!is_full()) *--sp = val;
    else fprintf(stderr, "stack overflow\n");
}

int pop() {
    if (!is_empty()) return *sp++;
    else fprintf(stderr, "stack underflow\n"); return 0;
}

void eval(const char* word) {
    // printf("Eval: %s\n", word);
    // eval
    // dup
    // swap
    // here
    // emit

    if (strcmp(word, "+") == 0) {
        int a = pop();
        int b = pop();
        printf("b = %d, a = %d\n", b, a); 
        push(a + b);
        print_stack();
    } else

    if (strcmp(word, "-") == 0) {
        int a = pop();
        int b = pop();
        printf("b = %d, a = %d\n", b, a); 
        push(b - a);
        print_stack();
    } else

    if (strcmp(word, "=") == 0) {
        push(pop() == pop() ? -1 : 0);
        print_stack();
    }

    if (strcmp(word, ".") == 0) {
        printf("%d", *sp);
        print_stack();
    } else

    if (strcmp(word, "dup") == 0) {
        push(*sp);
        print_stack();
    } else

    if (strcmp(word, "drop") == 0) {
        pop();
        print_stack();
    } else

    if (strcmp(word, "swap") == 0) {
        int a = pop();
        int b = pop();
        push(a);
        push(b);
        print_stack();
    } else

    if (strcmp(word, "over") == 0) {
        int a = pop();
        int b = pop();
        push(b);
        push(a);
        push(b);
        print_stack();
    } else

    if (strcmp(word, "ps") == 0) {
        print_stack();
    } else

    if (strcmp(word, "br") == 0) {
        printf("\n");
    }
}

void fth_readline() 
{
    static char inbuf[INBUF_SIZE];

    while(printf("> "), fgets(inbuf, INBUF_SIZE, stdin)) {
        inbuf[strcspn(inbuf, "\n")] = '\0';
        char* word = strtok(inbuf, " \t");
        while (word) {
            if (isdigit(word[0]) || (word[0] == '-' && isdigit(word[1]))) { // todo: change to is_number
                printf("push: %d\n", atoi(word));
                push(atoi(word));
                print_stack();
            } else {
                if (strcmp(word, "bye") == 0) return;
                else eval(word);
            }

            word = strtok(NULL, " \t");
        }
    }
}

int main(void) 
{
    sp = s0 + DS_SIZE;
    
    fth_readline();
    return 1;
}
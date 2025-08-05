#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define DS_SIZE 10
#define INBUF_SIZE 256
#define MAX_WORDS 16

typedef uintptr_t cell;

int sp;
int ds[10];

void print_stack(int s[], int sp) {
    printf("ds: [ ");
    for (int i = 0; i<sp; i++) printf("%d ", s[i]);
    printf("]\n");
}

int is_empty(int* sp) {
    return *sp == 0;
}

int is_full(int* sp) {
    return *sp == DS_SIZE;
}

void push(int s[], int* sp, int val) {
    if (!is_full(sp)) s[(*sp)++] = val;
}

void pop(int* sp) {
    if (!is_empty(sp)) --(*sp);
}

void eval(const char* word) {
    printf("Eval: %s\n", word);
}

void readline() {
    char input[INBUF_SIZE];
    char* words[MAX_WORDS];
    int word_count = 0;

    if(fgets(input, INBUF_SIZE, stdin)) {
        input[strcspn(input, "\n")] = '\0';

        char* token = strtok(input, " \t");
        while (token != NULL && word_count < MAX_WORDS) {
            words[word_count++] = token;
            token = strtok(NULL, " \t");
        }

        for (int i = 0; i < word_count; i++) {
            eval(words[i]);
        }
    }
}

void fth_readline() 
{
    static char inbuf[INBUF_SIZE];

    while(printf(">"), fgets(inbuf, INBUF_SIZE, stdin))
    {
        inbuf[strcspn(inbuf, "\n")] = '\0';
        char* word = strtok(inbuf, " \t");
        while (word) {
            if (isdigit(word[0])) { // todo: change to is_number later
                printf("push: %d\n", atoi(word));
                push(ds, &sp, atoi(word));
            } else {
                eval(word);
            }

            word = strtok(NULL, " \t");
        }
    }
}

int main(void) {
    // int sp;
    // int ds[10];
    // print_stack(ds, sp);
    // push(ds, &sp, 10);
    // push(ds, &sp, 158);
    // print_stack(ds, sp);
    // pop(&sp);
    // print_stack(ds, sp);
    // s0 = datastack;
    // sp = s0;

    // while (1) {
    //     printf(">");
    //     readline();
    // }
    fth_readline();
    
    return 1;
}
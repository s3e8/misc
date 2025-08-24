#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define DS_SIZE 1024
#define INBUF_SIZE 256
#define HERE_SIZE 10*1024*1024
#define MAX_WORDS 16
#define MAX_WORD_LEN 32

typedef uintptr_t cell;

typedef struct dict_hdr_t {
    cell flags;
    struct dict_hdr_t* next;
    char name[MAX_WORD_LEN];
} dict_hdr_t;

static void* here;
static void* here0;
static dict_hdr_t* latest;

static void** cfa(dict_hdr_t* word) {
    return (void**)(word + 1);
}

static void comma() {}
static void assemble_word() {}

static void* create_word(char* name, cell flags) {
    if (!name) name = "\0"; // for creating unnamed words

    dict_hdr_t* new = (dict_hdr_t*)here;
    here += sizeof(dict_hdr_t);

    strncpy(new->name, name, MAX_WORD_LEN);
    new->flags = flags;
    new->next = latest;
    latest = new;

    return new;
}

static void* create_constant() {}
static dict_hdr_t* find_word(char* word) {}
static void print_dict() {
    
}


cell* sp;
cell  s0[DS_SIZE];
float* fs;
float  f0[256];

void print_stack() {
    printf("[ ");
    for (cell* i = sp; i < s0 + DS_SIZE; i++) printf("%ld ", *i);
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
    else fprintf(stderr, "stack overflow\n"); // should cancel eval?
}

int pop() {
    if (!is_empty()) return *sp++;
    else fprintf(stderr, "stack underflow\n"); return 0; // should cancel eval?
}

void eval(const char* word) {
    // printf("Eval: %s\n", word);
    // eval
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

    if (strcmp(word, ">") == 0) {
        push(pop() > pop() ? 0 : -1);
        print_stack();
    }

    if (strcmp(word, "<") == 0) {
        push(pop() < pop() ? 0 : -1);
        print_stack();
    }

    if (strcmp(word, ".") == 0) {
        printf("%ld", *sp);
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
    here0 = malloc(HERE_SIZE);
    here = here0;
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

    free(here0);
}

int main(void) 
{
    sp = s0 + DS_SIZE;
    
    fth_readline();
    return 1;
}
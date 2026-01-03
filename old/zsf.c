#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// === CONFIG ===
#define RSTACKSIZE       256
#define DSTACKSIZE       256
#define WORD_NAME_LEN    32

// === TYPES ===
typedef uintptr_t cell;
typedef void (*XT)(void);  // Execution token - function pointer

// === MEMORY ===
static cell* here;         // Dictionary pointer
static cell* dp0;          // Start of dictionary
static size_t dict_size = 65536;  // 64KB dictionary

// === DICTIONARY STRUCTURE ===
typedef struct Word {
    struct Word* link;     // Link to previous word
    char name[WORD_NAME_LEN];
    cell flags;           // For later: immediate, hidden, etc.
    // Code field follows immediately
} Word;

static Word* latest = NULL;  // Latest defined word
static cell dstack[DSTACKSIZE];
static cell* dsp = dstack;   // Data stack pointer
static XT rstack[RSTACKSIZE];
static XT* rsp = rstack;     // Return stack pointer
static XT* ip = NULL;        // Instruction pointer

// === STACK OPERATIONS ===
#define PUSH(val) (*++dsp = (val))
#define POP() (*dsp--)
#define RPUSH(val) (*++rsp = (val))
#define RPOP() (*rsp--)

// === PRIMITIVES ===
void prim_exit(void) {
    ip = (XT*)RPOP();
}

void prim_lit(void) {
    PUSH((cell)*ip++);
}

void prim_dot(void) {
    cell n = POP();
    printf("%ld ", (long)n);
}

void prim_dup(void) {
    cell a = *dsp;
    PUSH(a);
}

void prim_swap(void) {
    cell a = POP();
    cell b = POP();
    PUSH(a);
    PUSH(b);
}

void prim_add(void) {
    cell b = POP();
    cell a = POP();
    PUSH(a + b);
}

void prim_bye(void) {
    exit(0);
}

// === DICTIONARY BUILDING ===
void comma(cell value) {
    *here++ = value;
}

Word* create_word(const char* name) {
    Word* word = (Word*)here;
    here = (cell*)((char*)here + sizeof(Word));
    
    strncpy(word->name, name, WORD_NAME_LEN);
    word->name[WORD_NAME_LEN-1] = '\0';
    word->link = latest;
    word->flags = 0;
    latest = word;
    
    return word;
}

void defcode(const char* name, XT code) {
    Word* word = create_word(name);
    comma((cell)code);  // Store code field
}

void defword(const char* name) {
    Word* word = create_word(name);
    comma((cell)prim_exit);  // Default code field - will be patched
}

// === INTERPRETER ===
Word* find_word(const char* name) {
    Word* w = latest;
    while (w != NULL) {
        if (strcmp(w->name, name) == 0) {
            return w;
        }
        w = w->link;
    }
    return NULL;
}

void execute_word(Word* word) {
    XT* code_field = (XT*)(word + 1);  // Point to code field
    (*code_field)();  // Execute the word's code
}

void execute_token(XT token) {
    token();  // Direct threading: call function pointer
}

// For indirect threading, you would use:
// void execute_token_indirect(Word* word) {
//     XT* code_field = (XT*)(word + 1);
//     RPUSH((XT)ip);      // Save IP on return stack
//     ip = code_field;    // Set IP to word's code field
//     NEXT();             // Start executing
// }

// === VM DISPATCH ===
void vm_init(void) {
    dsp = dstack - 1;      // Stack grows up
    rsp = rstack - 1;
    dp0 = (cell*)malloc(dict_size);
    here = dp0;
    
    // Define primitives
    defcode("exit", prim_exit);
    defcode("lit", prim_lit);
    defcode(".", prim_dot);
    defcode("dup", prim_dup);
    defcode("swap", prim_swap);
    defcode("+", prim_add);
    defcode("bye", prim_bye);
}

void interpret(const char* input) {
    char token[WORD_NAME_LEN];
    const char* p = input;
    
    while (*p) {
        // Skip whitespace
        while (*p == ' ' || *p == '\t' || *p == '\n') p++;
        if (!*p) break;
        
        // Extract token
        int i = 0;
        while (*p && *p != ' ' && *p != '\t' && *p != '\n' && i < WORD_NAME_LEN-1) {
            token[i++] = *p++;
        }
        token[i] = '\0';
        
        // Try to find word
        Word* word = find_word(token);
        if (word) {
            execute_word(word);
        } else {
            // Try to parse as number
            char* endptr;
            long num = strtol(token, &endptr, 10);
            if (*endptr == '\0') {
                // It's a number - push literal
                XT* saved_ip = ip;  // Save IP
                XT saved_rsp = *rsp; // Save RSP
                
                // Execute LIT primitive manually
                ip = (XT*)((char*)here);  // Temporary IP
                PUSH((cell)num);
                
                ip = saved_ip;
                *rsp = saved_rsp;
            } else {
                printf("? %s\n", token);
            }
        }
    }
}

// === MAIN ===
int main(void) {
    vm_init();
    
    printf("Simple Forth VM\n");
    printf("Type 'bye' to exit\n\n");
    
    char input[256];
    while (1) {
        printf(">> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        
        interpret(input);
        printf(" ok\n");
    }
    
    free(dp0);
    return 0;
}


/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// === CONFIG (change only this) ===
#define RSTACKSIZE          256
#define DSTACKSIZE          1056
#define WORD_NAME_MAX_LEN   64
#define USE_COMPUTED_GOTO   0
// === XT TYPE (auto-selects) ===
#if USE_COMPUTED_GOTO
typedef void* XT;                    // Label pointer
#else
typedef void (*XT)(void);            // Function pointer
#endif
// === DISPATCH MACRO (auto-selects) ===
#if USE_COMPUTED_GOTO
#define NEXT() goto **ip++           // Computed goto
#else  
#define NEXT() (*ip++)()             // Function call
#endif
// =================================

typedef uintptr_t   CELL;
typedef void (*OPCODE)();
typedef struct WORD WORD;
typedef struct THREAD THREAD;
typedef struct READER READER;
void defcode(char* name, void* code);

// Globals
void* here;
void* here0;
CELL  here_size;
WORD* latest = NULL;
OPCODE*     ip = NULL;
OPCODE*     returnstack[RSTACKSIZE]; // isnt this also used to store strings sometimes? or no... 
OPCODE**    rsp;

// RStack grows upward
#define PUSHRSP (val)   (*++rsp = (val))
#define POPRSP  ()      (*rsp--)




static void             op_comma(CELL value);
#define COMMA(val)      op_comma(val);
static WORD*            op_find(char* name);
#define FIND(name)      op_find(name);
static void            op_create(char* name);
#define CREATE(name)    op_create(name);

typedef struct WORD
{
    struct WORD*    next;
    char            name[WORD_NAME_MAX_LEN];
} WORD;

void defcode(char* name, void* code)
{
    CREATE  (name);
    // COMMA   ((CELL)code);
}

static void op_comma(CELL value)
{
    *(CELL*)here = value;
    here++;
}

static WORD* op_find(char* name)
{
    printf("finding: %s\n", name);

    WORD* word = latest;
    while (word != NULL)
    {
        if (strcmp(word->name, name) == 0)
        {
            printf("Found word: %s\n", name);
            return word;
        }

        word = word->next;
    }

    printf("word: %s not found...\n", name);
}

static void op_create(char* name)
{
    printf("creating: %s\n", name);

    if(!name) name = "\0";  // for creating unnamed words

    WORD* new = (WORD*)here;
    strncpy(new->name, name, WORD_NAME_MAX_LEN);
    new->next = latest;
    latest = new;

    here += sizeof(WORD);
}

static void op_dbg_print_stack()
{
    printf("stack: ERR: NO_STACK_IMPLEMENTED\n");
}

int main(void)
{
    char inbuf[256];

    CELL datastack[DSTACKSIZE];
    
    here_size = 10*1024*1024;
    here0 = malloc(here_size);
    here = here0;

    defcode("print-stack", (void*)op_dbg_print_stack);
    FIND("print-stack");

    // while(1)
    // {
    //     // prompt
    //     printf(">> ");

    //     if (fgets(inbuf, sizeof(inbuf), stdin) == NULL)
    //     {
    //         break; // Handle EOF (Ctrl+D on Unix, Ctrl+Z on Windows)
    //     }

    //     printf("\n%s", inbuf);

    // }
    
    free(here0);
    return 0;
}
*/
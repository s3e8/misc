#ifndef FTH_H
#define FTH_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FORTH_VERSION 0
// dictionary stuff
#define WORD_NAME_MAX_LENGTH 32
#define HERE_SIZE            10*1024*1024
// flags
#define BIT(x) (1<<(x)) // as opposed to (1<<x)??
#define FLAG_HIDDEN     BIT(0)
#define FLAG_IMMED      BIT(1)
#define FLAG_BUILTIN    BIT(2)
#define FLAG_HASARG     BIT(3)
#define FLAG_INLINE     BIT(4)
#define FLAG_DEFERRED   BIT(5)
// interpreter state
#define STATE_IMMEDIATE 0
#define STATE_COMPILE   1

/* the most important type, the cell. MUST be exactly of the pointer length! */
typedef uintptr_t cell;
typedef struct word_hdr_t       word_hdr_t;
typedef struct builtin_word_t   builtin_word_t;
typedef struct reader_state_t   reader_state_t;
typedef struct thread_state_t   thread_state_t;

#define MAX_COTHREADS 1 // call ctx?
typedef struct forth_vm_t
{
    thread_state_t* thread;
    word_hdr_t*     dictionary;
    reader_state_t* reader;
} forth_vm_t; 

// todo: reorg these
void init_forth();

typedef struct thread_state_t
{
    cell*   ds;
    cell*   s0;
    cell*   fs;
    cell*   f0;
    void*** rs;
    void*** r0;
    void**  ip;
} thread_state_t;

// typedef struct dict_state_t
// {
//     void*       here;
//     void*       here0;
//     cell        here_size;
//     word_hdr_t* latest;
// } dict_state_t;

// cell tmp;
extern void*        here;           /* working memory */                // why is this void* instead of cell
extern void*        here0;          /* beginning of working memory */   // same here, etc
extern cell         here_size;      /* size of allocated pool */        // why not initialize here?
extern word_hdr_t*  latest;

typedef struct word_hdr_t 
{ /* dictionary definition header. NEVER change the order of these fields, it's crucial! */
    cell                flags;
    struct word_hdr_t*  next; // rename to prev?
    char                name[WORD_NAME_MAX_LENGTH];
} word_hdr_t;

/* utility structure for creating builtins */
typedef struct builtin_word_t 
{
    char* name; // where are the names actually stored?
    cell  flags;
    void* code;
} builtin_word_t;

/// reader stuff ///
typedef struct reader_state_t 
{
    FILE* stream;
    char* linebuf;
    cell  linebuf_size;
    // wordbuf?
    char* next_char;
} reader_state_t;

// dict
static word_hdr_t*  create_word(const char* name, cell flags);
static word_hdr_t*  find_word(const char* name);
static void**       cfa(word_hdr_t* word);
static void         comma(cell value);
static void         create_builtin(builtin_word_t* b);
static void*        get_builtin_code(char* name);
static void         assemble_word(const char* name, cell flags, void** code, cell codesize);
static void         create_constant(const char* name, cell value);

// reader
extern void         init_reader_state(reader_state_t* state, char* linebuf, cell linebuf_size, FILE* fp);
reader_state_t*     reader_open_file(reader_state_t* reader, char* fname);
static void         skip_whitespace(reader_state_t* state);
static cell         is_eol(reader_state_t* state);
static cell         is_eof(reader_state_t* fp);
static char*        get_next_line(reader_state_t* state);
// prompt_line?
static int          read_key(reader_state_t* state); // todo: wtf does this do? ... or WHY, actually
extern char*        read_word(reader_state_t* state, char* tobuf); // todo: it's more like load_word_into_buffer
static void         emit_char(int c, FILE* fp);

// helpers
static void         create_constants();
static void         print_memory_stats();

// // ops - in the order they appear in this file
// void op_eow();
// void op_lit();
// void op_exit();
// void op_interpret();
// void op_branch();

// typedef struct forth_vm {
//     // register cell tmp
//     // int initialized
//     // ds
//     // s0
//     // rs
//     // r0
//     // 
// } forth_vm;
// forth_vm current;

//////////////////
//////////////////
//////////////////
//////////////////
/// stack stuff //
//////////////////
//////////////////
//////////////////
//////////////////
#define STACK_SIZE 1024

#define NEXT()      goto **ip++         //
#define PUSH(x)     *--ds = (cell)(x)   //
#define POP()       (*ds++)             //
#define TOP()       (*ds)               //
#define AT()        (*(ds + (x)))       //
#define INTARG()    ((cell)*ip++)       //
#define ARG()       (*ip++)             //
#define PUSHRS()    *--rs = (void**)(x) // 
#define POPRS()     (*rs++)             // 

/* utilies for calculating branch offsets in inline bytecode and referencing bytecodes */
#define OFFSET(x) (void*)((x) * sizeof(cell)) // do i need parens around x?
#define WORD(name) &&l_##name

// register cell   tmp; // todo: which register


#endif
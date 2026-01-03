#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// dictionary stuff
#define WORD_NAME_MAX_LENGTH 32
#define HERE_SIZE            10*1024*1024

#define BIT(x) (1<<(x)) // as opposed to (1<<x)??
#define FLAG_HIDDEN     BIT(0)
#define FLAG_IMMEDIATE  BIT(1)
#define FLAG_BUILTIN    BIT(2)
#define FLAG_HASARG     BIT(3)
#define FLAG_INLINE     BIT(4)
#define FLAG_DEFERRED   BIT(5)

#define STATE_IMMEDIATE 0
#define STATE_COMPILE   1

/* the most important type, the cell. MUST be exactly of the pointer length! */
typedef uintptr_t cell;
int cell_size = sizeof(uintptr_t);

typedef struct word_hdr_t {
    cell                flags;
    struct word_hdr_t*  next; // rename to prev?
    char                name[WORD_NAME_MAX_LENGTH];
} word_hdr_t;

/* utility structure for creating builtins */
typedef struct builtin_word_t {
    char* name; // where are the names actually stored?
    cell  flags;
    void* code;
} builtin_word_t;

// change void* to char* for portability?
/* free memory pointers and latest defined word */
static void* here;      /* working memory */                // why is this void* instead of cell
static void* here0;     /* beginning of working memory */   // same here, etc
static cell  here_size; /* size of allocated pool */        // why not initialize here?
static word_hdr_t* latest = NULL;

static word_hdr_t* create_word(const char* name, cell flags) {
    if (!name) name = "\0"; // for creating unnamed words // "\0" vs '\0'??

    word_hdr_t* new = (word_hdr_t*)here; // what happens in memory here?
    here += sizeof(word_hdr_t); // this is just regular integer addition integer, right?

    printf("new header:   %p (*%lu)\n", new, (uintptr_t)new);
    printf("new here:     %p (*%lu)\n", here, (uintptr_t)here);

    strncpy(new->name, name, WORD_NAME_MAX_LENGTH);
    new->flags  = flags;
    new->next   = latest; // rename next to prev in hdr struct?

    latest = new; // also whats going on in mem here? related to before in this func ofc
    return new;
}

// is this a "utility" function?
static word_hdr_t* find_word(const char* name) {
    if (!name) return NULL;

    word_hdr_t* hdr = latest;
    while (hdr)
    {
        if (
            !(hdr->flags & FLAG_HIDDEN) &&                  // If the word is NOT hidden...
            !strncmp(hdr->name, name, WORD_NAME_MAX_LENGTH) // ...and the strings DO match...
        ) return hdr;                                       // ...then return this word header.

        hdr = hdr->next;
    }

    return NULL;
}

static void** cfa(word_hdr_t* word) {
    return (void**)(word + 1);
}

static void comma(cell value) {
    *(cell*)here = value;
}

// runtime stuff
static void create_builtin(builtin_word_t* b) {
    create_word(b->name, b->flags | FLAG_BUILTIN);
    comma((cell)b->code);
}

static void* get_builtin_code(char* name) {
    word_hdr_t* hdr = find_word(name);
    return *(cfa(hdr));
}

static void assemble_word(const char* name, cell flags, void** code, cell codesize) {
    create_word(name, flags);
    for (int i = 0; i < codesize / sizeof(void*); i++) comma((cell)code[i]);
    comma((cell)get_builtin_code("eow"));
}

static void create_constant(const char* name, cell value) {
    void* flagdef[] = { get_builtin_code("lit"), (void*)value, get_builtin_code("exit") }; // why exit and not eow?
    assemble_word(name, FLAG_INLINE, flagdef, sizeof(flagdef));
}

/// init stuff ///
static void create_constants() {
    // create_constant("version",  FORTH_VERSION);

    create_constant("f_builtin",    FLAG_BUILTIN);
    create_constant("f_hasarg",     FLAG_HASARG);
    create_constant("f_immediate",  FLAG_IMMEDIATE);
    create_constant("f_hidden",     FLAG_HIDDEN);
    create_constant("f_inline",     FLAG_INLINE);
    create_constant("f_deferred",   FLAG_DEFERRED);
}

static void print_memory_stats() {
    // Calculate everything in bytes first
    uintptr_t base = (uintptr_t)here0;
    uintptr_t current = (uintptr_t)here;
    uintptr_t used_bytes = current - base;
    uintptr_t total_bytes = here_size;

    // Now calculate in cells (your fundamental unit)
    size_t cell_size = sizeof(cell);
    uintptr_t used_cells = used_bytes / cell_size;
    uintptr_t total_cells = total_bytes / cell_size;

    printf("Memory Block: %p -> %p\n", here0, (void*)(base + total_bytes));
    printf("Used:  %6lu bytes / %6lu cells\n", used_bytes, used_cells);
    printf("Free:  %6lu bytes / %6lu cells\n", total_bytes - used_bytes, total_cells - used_cells);
    printf("Total: %6lu bytes / %6lu cells\n", total_bytes, total_cells);

    // Print the current 'here' as an offset from the base
    printf("Current 'here' offset: %lu bytes / %lu cells\n", used_bytes, used_cells);
}

/// reader stuff ///


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

typedef struct thread_state_t {
    cell* ds;
    cell* s0;
    void*** rs;
    void*** r0;
} thread_state_t;

static thread_state_t* init_thread(cell* s0, void*** r0, void** entrypoint) {
    thread_state_t* new = malloc(sizeof(thread_state_t));
    new->s0 = s0;
    new->r0 = r0;
    new->ds = new->s0;
    new->rs = new->r0;

    // todo: implement multi green thread stuff

    return new;
}

static thread_state_t* create_thread(int ds_size, int rs_size, void** entrypoint) {
    return init_thread(
        (cell*)     malloc(ds_size * sizeof(cell)),
        (void***)   malloc(rs_size * sizeof(cell)),
        entrypoint
    );
}

static void kill_thread() {}

#define PUSH(x) *--ds = (cell)(x)   //
#define POP()   (*ds++)             //
#define TOP()   (*ds)               //
#define NEXT()  goto **ip++         //

/* utilies for calculating branch offsets in inline bytecode and referencing bytecodes */
#define OFFSET(x) (void*)((x) * sizeof(cell)) // do i need parens around x?
#define WORD(name) &&l_##name

static void test_shit(cell* ds, void*** rs, void*** ip) {
    create_word("hello", 0);
    if (find_word("hello")) printf("word: 'hello' found\n");
    
    PUSH(42);
    printf("top: %lu\n", TOP());
    POP();
    printf("top: %lu\n", TOP());
}

/////////////////
/// interpret ///
/////////////////
static void go_forth(cell* ds, void*** rs, void*** ip) {
    static int initialized = 0; // do we really need to do all this?

    register cell tmp;
    cell*   s0 = ds;
    void*** r0 = rs;

    char stdinbuf[1024];
    char wordbuf[WORD_NAME_MAX_LENGTH];
    char linebuf[WORD_NAME_MAX_LENGTH];

    cell state = STATE_IMMEDIATE;
    cell base  = 10;

    test_shit(ds, rs, NULL);
}

int main() {
    cell   datastack[1024];  // declare ds on stack
    void** returnstack[512]; // declare rs on stack

    here_size = 10*1024*1024;   // 10mb
    here0 = malloc(here_size);  // why not HERE_SIZE
    here  = here0;

    go_forth(datastack+1024, returnstack+512, NULL);


    ///  ///
    ///  ///
    free(here0);

    return 0; 
}
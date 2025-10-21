#include "fth0.h"
// #include <stdlib.h>
// #include <string.h>
// #include <ctype.h>

// // int cell_size = sizeof(uintptr_t);
// // todo: put in vm struct//  find vm for small lisp?
// // change void* to char* for portability?
// /* free memory pointers and latest defined word */
void*        here = NULL;       /* working memory */                // why is this void* instead of cell
void*        here0 = NULL;      /* beginning of working memory */   // same here, etc
cell         here_size = 0;     /* size of allocated pool */        // why not initialize here?
word_hdr_t*  latest = NULL;


static word_hdr_t* create_word(const char* name, cell flags) {
    if (!name) name = "\0"; // for creating unnamed words // "\0" vs '\0'??

    word_hdr_t* new = (word_hdr_t*)here; // what happens in memory here?
    here += sizeof(word_hdr_t); // this is just regular integer addition integer, right?

    printf("new header:   %p (*%lu)\n", new,    (uintptr_t)new);
    printf("new here:     %p (*%lu)\n", here,   (uintptr_t)here);

    strncpy(new->name, name, WORD_NAME_MAX_LENGTH);
    new->flags  = flags;
    new->next   = latest; // rename next to prev in hdr struct?

    latest = new; // also whats going on in mem here? related to before in this func ofc
    return new;     // why return "new" when latest is the same? or just pointer
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

    printf("Word: %s, not found\n", name);

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


// reader stuff
void init_reader_state(reader_state_t* state, char* linebuf, cell linebuf_size, FILE* fp) {
    state->stream       = fp;
    state->linebuf      = linebuf;
    state->linebuf[0]   = '\0';
    state->linebuf_size = linebuf_size;
    state->next_char    = linebuf;
}

// reader_state_t* reader_open_file(reader_state_t* reader, char* fname) {
//     reader->stream = open_file(fname, "r");
//     if (!reader->stream)
//     {
//         fprintf(stderr, "Cannot open file: %s!\n", fname);
//         return 1;
//     }
// }

char* get_next_line(reader_state_t* state) {
    char* tmp = fgets(state->linebuf, state->linebuf_size, state->stream);
    if(!tmp) return NULL;

    state->next_char = tmp;
    return tmp;
}

static void skip_whitespace(reader_state_t* state) {
    while(isspace(*state->next_char)) state->next_char++;
}

static cell is_eol(reader_state_t* state) {
    skip_whitespace(state);
    return *state->next_char=='\0';
}

static cell is_eof(reader_state_t* fp) {
    return *fp->next_char=='\0' && feof(fp->stream);
}

// static char *prompt_line(const char *prompt, reader_state_t *state) {}

static int read_key(reader_state_t* state) { // todo: ??
    if(*state->next_char == '\0') if(!get_next_line(state)) return -1;

    return *state->next_char++;
}

char* read_word(reader_state_t* state, char* tobuf) { // todo: it's more like buffer_word.. or load_word
    char* buf = tobuf; // charbuf?

    // skip whitespace first
    skipws:
        skip_whitespace(state);

    // buffer exhausted? fill and reskip whitespace
    if(*state->next_char == '\0') {
        if(!get_next_line(state)) return NULL;
        goto skipws; 
    }

    // copy until next whitespace
    while(*state->next_char != '\0' && !isspace(*state->next_char)) {
        *buf++ = *state->next_char++;
    }
    state->next_char++;
    *buf = '\0';

    return tobuf;
}

static void emit_char(int c, FILE* fp) {
    fputc(c, fp);
}


// thread stuff
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

// dbg
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
// static void fth_interpret_loop(cell* ds, void*** rs, void*** ip) 
// {
//     static int initialized = 0; // do we really need to do all this?

//     register cell   tmp;
//     cell*           s0 = ds;
//     void***         r0 = rs;
//     // builtin literals
//     cell            state = STATE_IMMEDIATE;
//     cell            base  = 10;

//     // void* builtin_immediatebuf[2] = { NULL, WORD(IRETURN) };
//     // void* word_immediatebuf[3] = { WORD(CALL), NULL, WORD(IRETURN) };

//     reader_state_t  stdin_state;
//     char            stdinbuf[1024];
//     char            wordbuf[WORD_NAME_MAX_LENGTH];
//     char            linebuf[WORD_NAME_MAX_LENGTH]; // why the same len?

//     // init builtins
//     static builtin_word_t* builtins[] = 
//     {   // todo: bytecode vs ops
//         #define BYTECODE(label, name, nargs, nfargs, flags, code ) { name, &&l_##label, flags },
//             #include "bytecodes.h"
//         #undef BYTECODE
//         { NULL, NULL, 0 }
//     };
//     // dyncall stuff

//     if (!initialized)
//     {
//         initialized = 1;

//         setvbuf(stdin, NULL, _IONBF, 0); // todo: what is buffering mode?
//         init_reader_state(&stdin_state, stdinbuf, 1024, stdin); // todo: linebuf_size const?

//         // builtin_word_t* b = builtins;
//         // while(b->name) create_builtin(b++);

//         // init constants
//         create_constant("version",          FORTH_VERSION);
//         create_constant("f_builtin",        FLAG_BUILTIN);
//         create_constant("f_hasarg",         FLAG_HASARG);
//         create_constant("f_immediate",      FLAG_IMMED);
//         create_constant("f_hidden",         FLAG_HIDDEN);
//         create_constant("f_inline",         FLAG_INLINE);
//         create_constant("f_deferred",       FLAG_DEFERRED);
//         create_constant("s0",               (cell) &s0);
//         create_constant("r0",               (cell) &r0);
//         // create_constant("t0",            (cell) &t0);
//         // create_constant("f0",            (cell) &f0);
//         create_constant("state",            (cell) &state);
//         create_constant("cellsize",         (cell)  sizeof(cell));
//         create_constant("floatsize",        (cell)  sizeof(float));
//         create_constant("base",             (cell) &base);
//         create_constant("here",             (cell) &here);
//         create_constant("here0",            (cell)  here0);
//         create_constant("hdrsize",          (cell)  sizeof(word_hdr_t));
//         create_constant("<stdin>",          (cell) &stdin_state);
//         create_constant("<stdout>",         (cell)  stdout);
//         // create_constant("input-stream",  (cell) &inputstate);
//         // create_constant("output-stream", (cell) &outp);
//         // create_constant("argc",          (cell)  argc);
//         // create_constant("argv",          (cell)  argv);
//         // create_constant("current-thread",   (cell) &current_thread);
//         // create_constant("debugger-vector",  (cell) &debugger_vector);

//         // finish initing constants

//         // init_thread(s0, r0, t0, ip);

//         // QUIT is the topmost interpreter loop: interpret forever. better version implemented in
//         // forth later that supports eof etc
//         void* quitcode[] = 
//         { 
//             WORD(INTERPRET), 
//             WORD(BRANCH), 
//             OFFSET(-2),
//             WORD(EOW)
//         };

//         ip = quitcode;
//     }

//     NEXT();

//     // unravel bytecode // todo: change l_ to op_?
//     #define BYTECODE(label, name, nargs, nfargs, flags, code) l_##label: code NEXT();
//     #include "bytecodes.h"

//     // test_shit(ds, rs, NULL);
// }

// static void fth_interpret_line(void** ip, cell* ds, void*** rs, reader_state_t* inputstate)
// {
//     reader_state_t  stdin_state;
//     char            stdinbuf[1024];
//     char            wordbuf[WORD_NAME_MAX_LENGTH];
//     char            linebuf[WORD_NAME_MAX_LENGTH]; // why the same len?
// tmp
//     // char* next_line = get_next_line(inputstate);
//     char* word = read_word(inputstate, linebuf);



// }

// int main() {
//     cell   datastack[1024];  // declare ds on stack
//     void** returnstack[512]; // declare rs on stack
//     // maybe also pass in s0 and r0?

//     here_size   = 10*1024*1024;   // 10mb
//     // here0       = malloc(here_size);  // why not HERE_SIZE, todo: rm malloc
//     here0       = 10*1024*1024;
//     here        = here0;

//     fth_interpret_loop(datastack+1024, returnstack+512, NULL);


//     ///  ///
//     ///  ///
//     // free(here0);

//     return 0; 
// }

// static void print_memory_stats() {
//     // Calculate everything in bytes first
//     uintptr_t base = (uintptr_t)here0;
//     uintptr_t current = (uintptr_t)here;
//     uintptr_t used_bytes = current - base;
//     uintptr_t total_bytes = here_size;

//     // Now calculate in cells (your fundamental unit)
//     size_t cell_size = sizeof(cell);
//     uintptr_t used_cells = used_bytes / cell_size;
//     uintptr_t total_cells = total_bytes / cell_size;

//     printf("Memory Block: %p -> %p\n", here0, (void*)(base + total_bytes));
//     printf("Used:  %6lu bytes / %6lu cells\n", used_bytes, used_cells);
//     printf("Free:  %6lu bytes / %6lu cells\n", total_bytes - used_bytes, total_cells - used_cells);
//     printf("Total: %6lu bytes / %6lu cells\n", total_bytes, total_cells);

//     // Print the current 'here' as an offset from the base
//     printf("Current 'here' offset: %lu bytes / %lu cells\n", used_bytes, used_cells);
// }



// #define BYTECODE(label, name, nargs, nfargs, flags, code) { name, &&l_##label, flags }




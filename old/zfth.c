#include "zfth_ops.h"

typedef uintptr_t   cell;
typedef cell*       XT;     // Execution Token (pointer to codeword)

static cell*    rsp;
static cell*    rs0;
static cell*    dsp;
static cell*    ds0;
static XT*      ip;     // ?


/*
    stacks --
    -- ds
    -- s0
*/

/* Helpers / Bootstrap Ops --
    -- defword  - creates compound words written in Forth itself
    -- defcode  - creates primitive words written in native C       // todo?: rename to defop?
    -- defvar   - todo: creates a variable
    -- defconst - todo: creates a const

    -- cfa      - // todo? not a word, right?
*/
/* Engine Ops --
    -- cfa
    -- 
*/
/* Globals --
    // Dictionary //
    -- here_size    - size of allocated pool        // why not initialize here? .. MACRO? or is there use in having it as global?
    -- here         - working memory                // why is this void* instead of cell
    -- here0        - beginning of working memory   // same here, etc
    -- latest       - //
*/
/* Native Words --
    -- word
    -- find
    -- comma
    -- tick
    -- eow
*/

void defword    (const char* name, cell flags, void* code[], int wordcount);
void defcode    (const char* name, cell flags, void* code);
void defvar     (const char* name, cell flags, cell value);
void defconst   (const char* name, cell flags, cell value);


#define F_IMMED     0x80
#define F_HIDDEN    0x20
#define F_LENMASK   0x1f

// DEFWORD  - .macro defword  name, namelen, flags=0, label             - creates compound words written in Forth itself
// DEFCODE  - .macro defcode  name, namelen, flags=0, label             - creates primitive words written in native C       // todo: rename to defop?
// DEFVAR   - .macro defvar   name, namelen, flags=0, label, initial=0  - 
// DEFCONST - .macro defconst name, namelen, flags=0, label, value      - 

void defword(const char* name, cell flags, void* code[], int wordcount) 
{
    // Create header
    if (!name) name = '\0'; // for creating unnamed words
    
    word_hdr_t* new = (word_hdr_t*)here;
    here += sizeof(word_hdr_t);

    strncpy(new->name, name, WORD_NAME_MAX_LENGTH);
    new->flags  = flags;
    new->next   = latest;
    latest      = new;
    // End create header

    // thread opcodes
    for (int i = 0; i < wordcount; i++) comma((cell)code[i]);

    // A list of word pointers to follow
}

// Built-in word helpers
void defcode(const char* name, cell flags, void* code) 
{
    if (!name) name = '\0'; // for creating unnamed words
    
    // Create header
    word_hdr_t* new = (word_hdr_t*)here;
    here += sizeof(word_hdr_t);

    strncpy(new->name, name, WORD_NAME_MAX_LENGTH);
    new->flags  = flags;
    new->next   = latest;
    latest      = new;
    // End create header

    comma((cell)code); // store native c code pointer
}

void defvar(const char* name, cell flags, cell value)
{
    // Variable returns its address: [LIT][address][EXIT]
    cell addr = (cell)here;

    cell words[] = {};
    defword(name, flags, words, 3);

    comma(value);
}

void defconst(const char* name, cell flags, cell value)
{
    // cell words[] = {XT(op_lit), (cell)value, XT(op_exit)};
    cell words[] = {};
    defword(name, flags, words, 3);
}


cell         here_size  = 0;        /* size of allocated pool */        // why not initialize here?
void*        here       = NULL;     /* working memory */                // why is this void* instead of cell
void*        here0      = NULL;     /* beginning of working memory */   // same here, etc
word_hdr_t*  latest     = NULL;

// void defword (const char* name, cell flags, void* code[], int wordcount);
// void defcode (const char* name, cell flags, void* code);
// void defconst(const char* name, cell value, cell flags);
// void defvar  (const char* name, cell value, cell flags);
// op_find      (const char* name);
// op_comma     ();
// op_tick      ();
//
//
// op_bye       ();
// 


#define PUSH()
#define POP()


#define DOCOL()                             // the inner-interpreter... I think

/* Native Ops */
#define WORD()      op_word()               // read word
#define FIND(name)  op_find(name)
#define COMMA(val)  op_comma(val)
#define TICK()      op_tick()
#define EOW()       {}
// defcode "'",1,,TICK
//     call _WORD           // Get next word
//     call _FIND           // Find it in dictionary  
//     call _TCFA           // Convert to CFA
//     push %eax            // Push CFA on stack
//     NEXT


#define LIT()       { PUSH(((cell)(*ip++)));    }
#define EXIT()      { ip = POPRS();             }

#define QUIT        { }

// void op_lit() {
//     // When LIT executes, ip points to the literal value
//     // Example: [LIT][123][NEXT_WORD] - ip points to 123
//     cell value = *ip++;  // Read literal, advance to next word
//     push(value);         // Push literal onto stack
//     NEXT();              // Continue execution
// }

// Basic compilation ops
static void** cfa(word_hdr_t* word) {
    return (void**)(word + 1);
}

static void op_comma(cell value) {
    *(cell*)here = value;
    here++;
}










//////////
// MAIN //
//      //
//      //
//      //
//////////
int main(int argc, char** argv)
{
    // cell   datastack[1024]; /* parameter stack */
    // void** returnstack[512];

    // here_size = 0;
    // here0     = malloc(here_size);
    // here      = here0;
}

void initcodes()
{
    // Stack manipulation
    defcode("DROP",     0, DROP);
    defcode("SWAP",     0, SWAP);
    defcode("DUP",      0, DUP);
    defcode("OVER",     0, OVER);
    defcode("ROT",      0, ROT);
    defcode("-ROT",     0, NROT);
    defcode("2DROP",    0, TWODROP);
    defcode("2DUP",     0, TWODUP);
    defcode("2SWAP",    0, TWOSWAP);
    defcode("?DUP",     0, QDUP);

    // Arithmetic
    defcode("1+",   0, INCR);
    defcode("1-",   0, DECR);
    defcode("4+",   0, INCRFOUR);
    defcode("4-",   0, DECRFOUR);
    defcode("+",    0, ADD);
    defcode("-",    0, SUB);
    defcode("*",    0, MUL);
    defcode("/MOD", 0, DIVMOD);

    // Comparison (signed)
    defcode("=",    0, EQU);
    defcode("<>",   0, NEQU);
    defcode("<",    0, LT);
    defcode(">",    0, GT);
    defcode("<=",   0, LE);
    defcode(">=",   0, GE);

    // Comparison (unsigned/zero-based)
    defcode("0=",   0, ZEQU);
    defcode("0<>",  0, ZNEQU);
    defcode("0<",   0, ZLT);
    defcode("0>",   0, ZGT);
    defcode("0<=",  0, ZLE);
    defcode("0>=",  0, ZGE);

    // Bitwise operations
    defcode("AND",      0, AND);
    defcode("OR",       0, OR);
    defcode("XOR",      0, XOR);
    defcode("INVERT",   0, INVERT);

    // // Control flow
    // defcode("EXIT", 0, EXIT);
    // defcode("LIT", 0, LIT);

    // Memory operations
    defcode("!",    0, STORE);
    defcode("@",    0, FETCH);
    defcode("+!",   0, ADDSTORE);
    defcode("-!",   0, SUBSTORE);
    defcode("C!",   0, STOREBYTE);
    defcode("C@",   0, FETCHBYTE);

    // Memory movement
    defcode("CMOVE",    0, CCOPY);
    defcode("CMOVE>",   0, CMOVE);
}


// /* ---- DEFCODE - Native Ops ---- */ 
// // Stack manipulation
// void op_drop()    { POP(); NEXT(); }
// void op_swap()    { cell a = POP(); cell b = POP(); PUSH(a); PUSH(b); NEXT(); }
// void op_dup()     { cell a = POP(); PUSH(a); PUSH(a); NEXT(); }
// void op_over()    { cell a = POP(); cell b = POP(); PUSH(b); PUSH(a); PUSH(b); NEXT(); }
// void op_rot()     { cell a = POP(); cell b = POP(); cell c = POP(); PUSH(b); PUSH(a); PUSH(c); NEXT(); }
// void op_nrot()    { cell a = POP(); cell b = POP(); cell c = POP(); PUSH(a); PUSH(c); PUSH(b); NEXT(); } // -rot
// void op_twodrop() { POP(); POP(); NEXT(); }  // 2drop
// void op_twodup()  { cell a = POP(); cell b = POP(); PUSH(b); PUSH(a); PUSH(b); PUSH(a); NEXT(); } // 2dup
// void op_twoswap() { cell a = POP(); cell b = POP(); cell c = POP(); cell d = POP(); PUSH(b); PUSH(a); PUSH(d); PUSH(c); NEXT(); } // 2swap
// void op_qdup()    { cell a = POP(); PUSH(a); if(a != 0) PUSH(a); NEXT(); } // ?dup

// // Arithmetic
// void op_incr()     { cell a = POP(); PUSH(a + 1); NEXT(); }     // 1+
// void op_decr()     { cell a = POP(); PUSH(a - 1); NEXT(); }     // 1-
// void op_incrfour() { cell a = POP(); PUSH(a + 4); NEXT(); }     // 4+
// void op_decrfour() { cell a = POP(); PUSH(a - 4); NEXT(); }     // 4-
// void op_add()      { cell b = POP(); cell a = POP(); PUSH(a + b); NEXT(); } // +
// void op_sub()      { cell b = POP(); cell a = POP(); PUSH(a - b); NEXT(); } // -
// void op_mul()      { cell b = POP(); cell a = POP(); PUSH(a * b); NEXT(); } // *
// void op_divmod()   { cell b = POP(); cell a = POP(); PUSH(a % b); PUSH(a / b); NEXT(); } // /MOD

// // Comparison (signed)
// void op_equ()      { cell b = POP(); cell a = POP(); PUSH(a == b ? -1 : 0); NEXT(); } // =
// void op_nequ()     { cell b = POP(); cell a = POP(); PUSH(a != b ? -1 : 0); NEXT(); } // <>
// void op_lt()       { cell b = POP(); cell a = POP(); PUSH(a < b ? -1 : 0); NEXT(); }  // <
// void op_gt()       { cell b = POP(); cell a = POP(); PUSH(a > b ? -1 : 0); NEXT(); }  // >
// void op_le()       { cell b = POP(); cell a = POP(); PUSH(a <= b ? -1 : 0); NEXT(); } // <=
// void op_ge()       { cell b = POP(); cell a = POP(); PUSH(a >= b ? -1 : 0); NEXT(); } // >=

// // Comparison (unsigned/zero-based)
// void op_zequ()     { cell a = POP(); PUSH(a == 0 ? -1 : 0); NEXT(); }  // 0=
// void op_znequ()    { cell a = POP(); PUSH(a != 0 ? -1 : 0); NEXT(); }  // 0<>
// void op_zlt()      { cell a = POP(); PUSH(a < 0 ? -1 : 0); NEXT(); }   // 0<
// void op_zgt()      { cell a = POP(); PUSH(a > 0 ? -1 : 0); NEXT(); }   // 0>
// void op_zle()      { cell a = POP(); PUSH(a <= 0 ? -1 : 0); NEXT(); }  // 0<=
// void op_zge()      { cell a = POP(); PUSH(a >= 0 ? -1 : 0); NEXT(); }  // 0>=

// // Bitwise operations
// void op_and()      { cell b = POP(); cell a = POP(); PUSH(a & b); NEXT(); }  // AND
// void op_or()       { cell b = POP(); cell a = POP(); PUSH(a | b); NEXT(); }  // OR
// void op_xor()      { cell b = POP(); cell a = POP(); PUSH(a ^ b); NEXT(); }  // XOR
// void op_invert()   { cell a = POP(); PUSH(~a); NEXT(); }  // INVERT

// // Control flow
// void op_exit()     { ip = (cell*)POPRS(); NEXT(); }  // EXIT
// void op_lit()      { PUSH(*ip++); NEXT(); }          // LIT

// // Memory operations
// void op_store()    { cell addr = POP(); cell value = POP(); *(cell*)addr = value; NEXT(); } // !
// void op_fetch()    { cell addr = POP(); PUSH(*(cell*)addr); NEXT(); } // @
// void op_addstore() { cell addr = POP(); cell value = POP(); *(cell*)addr += value; NEXT(); } // +!
// void op_substore() { cell addr = POP(); cell value = POP(); *(cell*)addr -= value; NEXT(); } // -!
// void op_storebyte(){ cell addr = POP(); cell value = POP(); *(char*)addr = (char)value; NEXT(); } // C!
// void op_fetchbyte(){ cell addr = POP(); PUSH(*(char*)addr); NEXT(); } // C@

// // Memory movement
// void op_ccopy()    { cell n = POP(); cell src = POP(); cell dest = POP(); memcpy((void*)dest, (void*)src, n); NEXT(); } // CMOVE (forward)
// void op_cmove()    { cell n = POP(); cell src = POP(); cell dest = POP(); memmove((void*)dest, (void*)src, n); NEXT(); } // CMOVE> (backward-safe)
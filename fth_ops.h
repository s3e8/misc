#ifndef FTH_OPS_H
#define FTH_OPS_H

// -- %esi  - ip
// %eax  - Return Stack todo? or cach top of stack? idk
// %al   - 
// %ebp  - parameter stack pointer
// %esp  - 
// lodsl -

// F_IMMED      -
// F_HIDDEN     -
// F_LENMASK    -

// ip       - instruction pointer
// rs       - return stack
// r0       - return stack (top?)
// ds       - data stack / parameter stack / THE Stack
// s0       - data stack (top?)
// fs       - float stack (wait to impl)
// f0       - float stack (top?)

// NEXT()   - 
// EXIT()   -
// DOCOL()  - the interpreter!
// QUIT()   - 
// LABEL()  - used to be called WORD() in slarba's sforth
// -- PUSHRS()
// -- POPRS()
// -- 
// --
// -- INT() / INTARG() ??
// op_docol(); // ???

// DEFWORD  - .macro defword  name, namelen, flags=0, label             - creates compound words written in Forth itself
// DEFCODE  - .macro defcode  name, namelen, flags=0, label             - creates primitive words written in native C       // todo: rename to defop?
// DEFVAR   - .macro defvar   name, namelen, flags=0, label, initial=0  - 
// DEFCONST - .macro defconst name, namelen, flags=0, label, value      - 

typedef struct word_hdr_t word_hdr_t;

word_hdr_t* defword();  /* -- creates compound words written in Forth itself    -- */ 
word_hdr_t* defcode();  /* -- creates primitive words written in native C       -- */ 
word_hdr_t* defvar();   /**/
word_hdr_t* defconst(); /**/

/* ---- DEFCODE - Native Ops ---- */ 
// DROP
// SWAP
// DUP
// OVER
// ROT
// -ROT
// 2DROP
// 2DUP
// 2SWAP
// QDUP     (?DUP)
// INCR     (1+)
// DECR     (1-)
// INCR4    (4+)
// DECR4    (4-)
// ADD      (+)
// SUB      (-)
// MUL      (*)
// ??
// DIVMOD   (/MOD)
// EQU
// NEQU
// LT
// GT
// LE
// GE
// ZEQU
// ZNEQU
// ZLT
// ZGT
// ZLE
// ZGE
// AND
// OR
// XOR
// INVERT
// EXIT
// LIT
// STORE
// FETCH
// ADDSTORE
// SUBSTORE
// STOREBYTE
// FETCHBYTE
// CCOPY
// CMOVE

/* ---- DEFVAR - Built-in Forth Variables ---- */
// STATE
// LATEST
// HERE
// S0
// BASE

/* ---- DEFCONST - Built-in Forth Constants ---- */
// VERSION		Is the current version of this FORTH.
// R0		    The address of the top of the return stack.
// DOCOL		Pointer to DOCOL.
// F_IMMED		The IMMEDIATE flag's actual value.
// F_HIDDEN	    The HIDDEN flag's actual value.
// F_LENMASK	The length mask in the flags/len byte.
//
// SYS_*		and the numeric codes of various Linux syscalls (from <asm/unistd.h>)

/* RETURN STACK */
/* ---- DEFCODE - Native Ops ---- */
// TOR
// FROMR
// RSFETCH / RSPFETCH
// RSSTORE / RSPSTORE
// RDROP

/* PARAMETER (DATA) STACK */
/* ---- DEFCODE - Native Ops ---- */
// DSPFETCH
// DSPSTORE

/* INPUT AND OUTPUT */
/* ---- DEFCODE - Native Ops ---- */
// KEY
// EMIT
// WORD
// NUMBER

/* DICTIONARY LOOK UPS */
/* ---- DEFCODE - Native Ops ---- */
// FIND
// TCFA / CFA todo: ??
// TDFA / DFA todo: ??

/* COMPILING */
/* ---- DEFCODE - Native Ops ---- */
// CREATE
// COMMA
// LBRAC
// RBRAC
// COLON        (depends-on CREATE DOCOL)
// SEMICOLON

/* EXTENDING THE COMPILER */
/* ---- DEFCODE - Native Ops ---- */
// IMMEDIATE todo: look-up and review syntax conventions in jonesforth
// HIDDEN -- 'addr HIDDEN' toggles the hidden flag (F_HIDDEN) of the word defined at addr todo: look-up usage
// HIDE -- 'HIDE word' toggles the flag on a named 'word'
// TICK

/* BRANCHING */
/* ---- DEFCODE - Native Ops ---- */
// BRANCH   -- unconditional branch
// ZBRANCH  -- conditional branch (it only branches if the top of stack is zero)

/* LITERAL STRINGS */
/* ---- DEFCODE - Native Ops ---- */
// LITSTRING
// TELL

/* QUIT AND INTERPRET */
/* ---- DEFCODE - Native Ops ---- */
// QUIT
// INTERPRET -- This interpreter is pretty simple, but remember that in FORTH you can always override it later with a more powerful one!

/* ODDS AND ENDS */
/* ---- DEFCODE - Native Ops ---- */
// CHAR
// EXECUTE
// SYSCALL3 - numbers indicate nargs
// SYSCALL2 - numbers indicate nargs
// SYSCALL1 - numbers indicate nargs
// SYSCALL0 - numbers indicate nargs

/* DATA SEGMENT */ // todo: look-up/review this part in jforth
/* ---- Runtime Globals ---- */
// DATA_SEGMENT? / DICTIONARY

/* MY SHIT (ASM Primitives (brk(), etc), Misc) */

/* START OF FORTH CODE */
// ...
// ...
// ...



















#endif // FTH_OPS_H

// -- DEFCODE -- for defining primitives
//  .macro defcode name, namelen, flags=0, label
// #define DECODE(label, name, namelen?, flags=0)
// DROP
// SWAP 
// DUP
// OVER
// ROT
// -ROT     (label: NROT)
// 2DROP    (label: TWODROP)    // drop top two elements of stack
// 2DUP     (label: TWODUP)     // duplicate top two elements of stack
// 2SWAP    (label: TWOSWAP)    // swap top two pairs of elements of stack
// ?DUP     (label: QDUP)       // duplicate top of stack if non-zero
// 1+ (label: INCR)
// 1- (label: DECR)
// 4+ INCR4
// 4- DECR4
// + ADD
// - SUB
// * MUL
// /MOD DIVMOD todo? should this be defword and not defcode prim?
// = EQU // top two words are equal?
// <> NEQU // top two words are not equal?
// < LT
// > GT
// <= LE
// >= GE
// 0= ZEQU // top of stack equals 0?
// 0<> ZNEQU  // top of stack not 0?
// 0< ZLT     // comparisons with 0
// 0> ZGT
// 0<= ZLE
// 0>= ZGE
// AND    // bitwise AND
// OR     // bitwise OR
// XOR    // bitwise XOR
// INVERT // this is the FORTH bitwise "NOT" function (cf. NEGATE and NOT)
///
/// MEMORY CODES!
// ! STORE 



// -- DEFWORD -- for defining words
//  .macro defword name, namelen, flags=0, label
// #define DEFWORD(label, name, namelen?, flags=0)
//  defword "DOUBLE",6,,DOUBLE
//  .int DUP,PLUS,EXIT
//



// /* Flags - these are discussed later. */
//      .set F_IMMED,   0x80
//      .set F_HIDDEN,  0x20
//      .set F_LENMASK, 0x1f     // length mask
// #define BIT(x) (1<<(x)) // as opposed to (1<<x)??
// #define FLAG_HIDDEN     BIT(0)
// #define FLAG_IMMED      BIT(1)
// #define FLAG_BUILTIN    BIT(2)
// #define FLAG_HASARG     BIT(3)
// #define FLAG_INLINE     BIT(4)
// #define FLAG_DEFERRED   BIT(5)

///////////////
/* IDK WORDS */
///////////////
// _start?
// _cold_start?
// -- EXIT
// -- DUP
// -- +
// -- -

///////////////
/* REGISTERS */
///////////////
// ESP Stack Pointer
// EBP Frame Pointer
// -- IP?
//

//////////////////
/* ASSEMBLY OPS */
//////////////////
// GOTO?
// JMP / JUMP
// CALL

///////////////////
/* FORTH OPCODES */ // "FORTH-ISMS" as put by jonesforth
///////////////////
// Primitives        - Every FORTH primitive that we write has to be ended by NEXT. Think of it kind of like a return.
// DOCOL
// LIT - push to ds or rs?
// EXIT
// Return Stack
// Param Stack
// Interpreter
// Ops / Codewords   - The codeword is a pointer to the interpreter to run the function. todo??
// etc
//
//
// NEXT
// PUSHRS / PUSHRSP
// POPRS  / POPRSP
// DOCOL  / THE INTERPRETER!

// -- NEXT
// /* NEXT macro. */
//      .macro NEXT
//          lodsl
//          jmp *(%eax)
//      .endm

// /* Macros to deal with the return stack. */
// -- PUSHRS / PUSHRSP
//      .macro PUSHRSP reg
//          lea -4(%ebp),%ebp       // push reg on to return stack
//          movl \reg,(%ebp)
//      .endm
// -- POPRS / POPRSP
//      .macro POPRSP reg
//          mov (%ebp),\reg         // pop top of return stack to reg
//          lea 4(%ebp),%ebp
//      .endm

// /* DOCOL - the interpreter! */
//      .text
//      .align 4
//  DOCOL:
//      PUSHRSP %esi            // push %esi on to the return stack
//      addl $4,%eax            // %eax points to codeword, so make
//      movl %eax,%esi          // %esi point to first data word
//      NEXT

// -- CALL
// BYTECODE(CALL, "call", 0, 0, FLAG_HASARG, { 
//     void *fn = ARG();
//     PUSHRS(ip);
//     ip = fn;    
// })
//
// -- 


// misc
// '1f' (etc.) means label '1:' "forwards" and '1b' (etc.) means label '1:' "backwards"
// 'ja' is "jump if above", 'jb' for "jump if below", 'je' "jump if equal" etc.
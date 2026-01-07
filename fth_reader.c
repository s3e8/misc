#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// config //
#define WORD_NAME_MAX_LENGTH    64
#define HERE_SIZE               10*1024*1024 // 10mb
#define NESTING_STACK_MAX_DEPTH 64
#define DEFAULT_LINEBUF_SIZE    256

#include <stdint.h>
typedef uintptr_t cell;

// dictionary //
typedef struct word_hdr_t
{
    cell                flags;
    struct word_hdr_t*  next;
    char                name[WORD_NAME_MAX_LENGTH];
} word_hdr_t;

static void*            here;
static void*            here0;
static cell             here_size;
static word_hdr_t*      latest = NULL;

static word_hdr_t* find(char* name)
{
    if (!name) return NULL;

    // printf("finding: %s...\n", name);

    word_hdr_t* hdr = latest;
    while (hdr)
    {
        if (!strncmp(hdr->name, name, WORD_NAME_MAX_LENGTH))
        {
            // printf("found\n");
            return hdr;
        }

        hdr = hdr->next;
    }

    // printf("word not found\n");
    return NULL;
}

static word_hdr_t* create(char* name, cell flags)
{
    if (!name) name = "\0";

    // printf("creating: %s\n", name);

    word_hdr_t* new = (word_hdr_t*)here;
    here += sizeof(word_hdr_t);
    strncpy(new->name, name, WORD_NAME_MAX_LENGTH);
    new->flags = flags;
    new->next = latest;
    latest = new;

    // if (find(name)) printf("created successfully\n");
    // else printf("creation failed\n");
    
    return new;
}

static void** cfa(word_hdr_t* word)
{
    return (void**)(word + 1);
}

static void comma(cell value)
{
    *(cell*)here = value;
    here += sizeof(cell);
}

// word header flags //
#define BIT(x) (1<<(x))
#define FLAG_HIDDEN     BIT(0)
#define FLAG_IMMEDIATE  BIT(1)
#define FLAG_BUILTIN    BIT(2)
#define FLAG_HASARG     BIT(3)
#define FLAG_INLINE     BIT(4)
#define FLAG_DEFERRED   BIT(5)

// compiler state //
#define STATE_IMMEDIATE 0
#define STATE_COMPILE   1

void** ip = NULL;

#define NEXT() goto **ip++
#define ARG()       ( *ip++)
#define INTARG()    ((cell)(*ip++))
#define PUSHRS(x)   (*--rs = (void**)(x))
#define POPRS()     (*rs++)
// stack stuff //
#define PUSH(x)   (*ds++ = (cell)(x))
#define POP()     (*--ds)
#define PEEK()    (*(ds-1))

// helper macros //
#define CODE(name) &&op_##name
#define OP(name)     op_##name
/* utility for calculating branch offsets in inline bytecode and referencing bytecodes */
#define OFFSET(x) (void*)((x)*sizeof(cell))

// core bootstrap helpers //
void* getcode(char* name)
{
    word_hdr_t* word = find(name);
    return *(cfa(word));
}

void defcode(char* name, void* code, cell flags, int nargs)
{
    create(name, flags | FLAG_BUILTIN);
    comma((cell)code);
}

void defword(char* name, void* code[], int wordcount, cell flags)
{
    // printf("compiling word: %s\n", name);
    
    create(name, flags);
    // don't need to do this because of word immediatebuf?
    // that sounds incorrect
    // comma((cell)getcode("docol"));

    // slarba does: codesize/sizeof(void*)?
    for (int i = 0; i < wordcount; i++) comma((cell)code[i]);

    // dont think we need this either?
    // comma((cell)getcode("eow"));

    // todo: add code to also add pointer as name that saves string of word name it points to
}

void defconst(char* name, cell value)
{
    // docol pushes the address of the code to ip
    // lit pushes the next value on ip to the stack
    // exit sets ip to the return stack top... I think
    void* flagdef[] = { getcode("lit"), 0, getcode("exit") };
    flagdef[1] = (void*)value;
    defword(name, flagdef, 3, FLAG_INLINE);
}

void print_stack(cell* ds, cell* s0)
{
    printf("[");
    for (cell* p = ds - 1; p >= s0; p--)
    {
        printf(" %ld", (long)*p);
    }
    printf(" ]\n");
}

// reader stuff //
typedef struct reader_state_t
{
    FILE*   stream;
    char*   current_line;
    cell    current_line_size;
    char*   current_word;
    cell    current_word_size;
    char*   cursor;
} reader_state_t;

// reader state functions //
void init_reader_state(reader_state_t* rs, char* current_line, cell current_line_size, FILE* stream)
{
    // reader_state_t* rs = (reader_state_t*)malloc(sizeof(reader_state_t));
    rs->stream              = stream;
    rs->current_line        = current_line;
    rs->current_line[0]     = '\0';
    rs->current_line_size   = current_line_size;
    rs->cursor              = current_line;
}

static cell is_space(char c)
{
    return c == ' ' || c == '\t';
}

static void skip_whitespace(reader_state_t* rs)
{
    while (is_space(*rs->cursor))
    {
        rs->cursor++;
    }
}

static cell is_eol(reader_state_t* rs)
{
    skip_whitespace(rs);
    return *rs->cursor == '\0' || *rs->cursor == '\n';
}

static cell is_eof(reader_state_t* rs)
{
    return *rs->cursor=='\0' && feof(rs->stream);
}

static char* get_next_line(reader_state_t* rs)
{
    char* tmp = fgets(rs->current_line, rs->current_line_size, rs->stream);
    if (!tmp) return NULL;

    // Remove newline if present
    size_t len = strlen(rs->current_line);
    if (len > 0 && rs->current_line[len-1] == '\n')
    {
        rs->current_line[len-1] = '\0';
    }
    
    rs->cursor = rs->current_line;

    return rs->current_line;
}

static void prompt_line(char* prompt, reader_state_t* rs)
{
    printf("%s", prompt);
    fflush(stdout);

    // get_next_line(rs);
}

static char* read_word(reader_state_t* rs, char* tobuf)
{
    // Skip whitespace
    skipws:
        skip_whitespace(rs);
    
    // If at end of line, read new line
    if (*rs->cursor == '\0') 
    {
        prompt_line("> ", rs);
        if (!get_next_line(rs)) return NULL;
        goto skipws;
    }
    
    // If still empty, return NULL
    if (*rs->cursor == '\0') return NULL;

    char* buf = tobuf;
    int count = 0;
    int bufsize = WORD_NAME_MAX_LENGTH;
    
    // Copy word until next whitespace or end
    while (*rs->cursor != '\0' && !is_space(*rs->cursor) && count < bufsize - 1) 
    {
        *buf++ = *rs->cursor++;
        count++;
    }
    rs->cursor++;
    *buf = '\0';
    
    return tobuf;
}

void forth_run(void*** rs, cell* ds, int argc, char** argv)
{
    void* code_immediatebuf[2] = { NULL, CODE(IRETURN) };
    void* word_immediatebuf[3] = { CODE(DOCOL), NULL, CODE(IRETURN) };

    // i think this is for re-entering the interpreter after immediate execution??
    // as opposed to rs which is for lots of stuff
    void**  nestingstack_space[NESTING_STACK_MAX_DEPTH];
    void*** nestingstack = nestingstack_space + NESTING_STACK_MAX_DEPTH;

    // char stdinbuf[1024];
    char linebuf[DEFAULT_LINEBUF_SIZE];
    char wordbuf[WORD_NAME_MAX_LENGTH];

    
    reader_state_t reader_state;
    init_reader_state(&reader_state, linebuf, DEFAULT_LINEBUF_SIZE, stdin);

    void*** r0 = rs;
    cell*   s0 = ds;

    register cell tmp;

    // vars and consts //
    cell base  = 10;
    cell state = 0;

    // define primitive bytecode (opcode?) //
    ////// char* name ///// code ////////////// flags ///////// nargs //
    defcode("interpret",    CODE(INTERPRET),    0,              0);
    defcode("branch",       CODE(BRANCH),       FLAG_HASARG,    1);
    defcode("docol",        CODE(DOCOL),        FLAG_HASARG,    1); // in slarba's simpleforth.. this is "CALL"
    defcode(":",            CODE(COLON),        FLAG_IMMEDIATE, 0); // is this supposed to be immediate? its not in forth bs code
    defcode(";",            CODE(SEMICOLON),    FLAG_IMMEDIATE, 0);
    defcode("ireturn",      CODE(IRETURN),      0,              0); // returns from immediatebuf and nestingstack .. I think
    defcode("lit",          CODE(LIT),          FLAG_HASARG,    1);
    defcode("eow",          CODE(EOW),          0,              0);
    defcode("exit",         CODE(EXIT),         0,              0); // returns from return stack .. I think
    defcode("create",       CODE(CREATE),       FLAG_HASARG,    1);
    defcode(".",            CODE(DOT),          0,              0);
    defcode("bye",          CODE(BYE),          0,              0);

    // define variables and constants //
    defconst("base",    10);
    defconst("state",   0);
    defconst("here",    (cell)here);
    defconst("here0",   (cell)here0);
    defconst("latest",  (cell)latest);
    //
    defconst("s0", (cell)&s0);
    defconst("r0", (cell)&r0);
    //
    // defconst("input-state",    (cell)&input_state);
    // defconst("output-state",   (cell)&output);

    // ---- OUTER INTERPRETER ---- //
    // QUIT is the topmost interpreter loop: interpret forever. better version implemented in
    // forth later that supports eof etc
    void *quitcode[] = 
    { 
        CODE(INTERPRET), 
		CODE(BRANCH), OFFSET(-2),
	    CODE(EOW)
    };
    ip = quitcode;

    NEXT();

    // prevent fallthrough to labels
    return;

    // primitives //
    OP( INTERPRET ):
        {
            // printf("stdinbuf:  %s\n", stdinbuf);
            // printf("linebuf:   %s\n", reader_state.linebuf);
            // printf("wordbuf:   %s\n", wordbuf);
            // printf("next char: %s\n", reader_state.next_char);

            // //
            // strncpy(wordbuf, read_word(&reader_state, wordbuf), WORD_NAME_MAX_LENGTH);
            // if (!wordbuf)
            // {
            //     NEXT();
            // }

            // Read a line if we don't have one already
            // if (*reader_state.cursor == '\0')
            // {
            //     prompt_line("> ", &reader_state);
            //     read_word(&reader_state, linebuf);
            // }

            strncpy(wordbuf, read_word(&reader_state, linebuf), WORD_NAME_MAX_LENGTH);

            // printf("stdinbuf:  %s\n", stdinbuf);
            printf("rs.linebuf: %s\n", reader_state.current_line);
            printf("linebuf:    %s\n", linebuf);
            printf("wordbuf:    %s\n", wordbuf);
            printf("cursor:     %s\n", reader_state.cursor);

            wordbuf[strcspn(wordbuf, "\n")] = '\0';
            if (wordbuf[0] == '\0') NEXT();

            word_hdr_t* word = find(wordbuf);
            if (!word)
            {
                // check if words a number
                char* endptr = NULL;
                cell val = (cell)strtol(wordbuf, &endptr, (int)base);
                
                // if it's not a number, the end pointer will not be a line end
                if (*endptr != '\0') 
                {                    
                    printf("unknown word: %s\n", wordbuf);
                    NEXT();
                }
                else // else it's a number //
                {
                    if (state == STATE_COMPILE)
                    {
                        // compile number as lit
                        printf("compiling number: %d\n", (int)val);
                        comma((cell)getcode("lit"));
                        comma((cell)val);

                    }
                    else // else it's in immediate mode
                    {
                        // push number to stack
                        printf("number: %d\n", (int)val);
                        PUSH(val);
                    }
                }
                print_stack(ds, s0);
                NEXT();
            }

            if (state == STATE_COMPILE && !(word->flags & FLAG_IMMEDIATE))
            {
                if (word->flags & FLAG_BUILTIN)
                {
                    // compile as opcode
                    comma((cell)*cfa(word));
                }
                else
                {
                    // comma((cell) CODE(DOCOL)); // why would we need this?
                    comma((cell) cfa((word)));
                }
            }
            // else it's in immediate state (or word has immediate flag)
            // thus we schedule and interpret the word
            else
            {
                // printf("scheduling word: %s\n", word->name);
                // printf("flags: %lu\n", word->flags);
                // printf("code addr: %p\n", cfa(word));

                // retrieve the start of this words code
                void** code = cfa(word);
                // todo: comment
                *--nestingstack = ip;

                // if it's a builtin (defcode), we use code_immediatebuf
                if (word->flags & FLAG_BUILTIN)
                {
                    // printf("executing builtin word\n");

                    code_immediatebuf[0] = *code;
                    ip = code_immediatebuf; // NEXT() will be called after
                }

                // else it's a composite word (defword), we use word_immediatebuf
                else
                {
                    printf("executing composite word\n");

                    word_immediatebuf[1] = (void*)code; // todo: why the case vs the dereference for builtins?
                    ip = word_immediatebuf;
                }

                NEXT();
            }
        }
        NEXT();

    OP( PROMPT ):
        {
            printf("[ prompt ]\n");

            reader_state_t* state = (reader_state_t*)POP();
            char* prompt = (char*)POP();
            prompt_line(prompt, &reader_state);
        }
        NEXT();

    OP( BRANCH ):
        {
            // printf("[ branch ]\n");
            tmp = INTARG();
            ip += (tmp/sizeof(void*))-1;
        }
        NEXT();

    OP( EOW ):
        {
            printf("[ eow ]\n");
        }
        NEXT();
    
    OP( DOCOL ): // in slarba's simpleforth.. this is "CALL"
        {
            printf("[ docol ]\n");

            void* fn = ARG();
            printf("pushing to return stack: %p\n", ip);
            PUSHRS(ip);
            ip = fn;
        }
        NEXT();

    OP( COLON ):
        {
            printf("[ colon ]\n");
            
            // Read the word name
            if (!read_word(&reader_state, wordbuf)) {
                printf("Failed to read word name\n");
                NEXT();
            }
            
            printf("Creating word: %s\n", wordbuf);
            
            // Create the word header
            create(wordbuf, 0);
            latest->flags |= FLAG_HIDDEN;
            
            // Start the word with DOCOL
            comma((cell)getcode("DOCOL"));
            
            // Enter compilation mode
            state = STATE_COMPILE;
            printf("Entered compilation mode for word: %s\n", wordbuf);
        }
        NEXT();
        // {
        //     printf("[ colon ]\n");

        //     char name[WORD_NAME_MAX_LENGTH];
            
        //     // read next word
        //     read_word(&reader_state, linebuf);

        //     name[strcspn(name, "\n")] = '\0';
        //     if (name[0] == '\0') NEXT();

        //     create(name, 0);
        //     printf("created word: %s\n", name);
        //     // comma((cell)getcode("docol"));
        //     latest->flags |= FLAG_HIDDEN;
        //     state = STATE_COMPILE;
        // }
        // NEXT();

    OP( SEMICOLON ):
        {
            printf("[ semicolon ]\n");

            // finish current definition
            comma((cell)getcode("exit"));
            // comma((cell)getcode("ireturn"));
            latest->flags &= ~FLAG_HIDDEN;

            

            state = STATE_IMMEDIATE;
        }
        NEXT();
    
    OP( LIT ):
        {
            printf("[ lit ]\n");
            PUSH(INTARG());
            printf("literal arg: %d\n", (int)INTARG());
        }
        NEXT();

    OP( EXIT ):
        {
            printf("[ exit ]\n");
            ip = POPRS();
            // printf("popping return stack to ip %p\n", ip);
        }
        NEXT();

    OP( IRETURN ):
        {
            // printf("[ ireturn ]\n");

            ip = *nestingstack++;
            // printf("popping nesting stack to ip %p\n", ip);
        }
        NEXT();

    OP( CREATE ):
        {
            printf("[ create ]\n");

            // create((char*)POP(), 0);
        }
        NEXT();

    OP( DOT ):
        {
            // printf("[ . ]\n");

            cell val = POP();
            printf("%ld\n", (long)val);
        }
        NEXT();

    OP( BYE ):
        return;
}

int main(int argc, char** argv)
{
    cell   datastack[1024];
    void** returnstack[512];

    // todo: should I initialize reader state stuff on the stack here or in interpret?
    // todo: startup.f

    // init dictionary
    here_size   = HERE_SIZE;
    here0       = malloc(here_size);
    here        = here0;

    // + sizeofstack because they grow downward
    forth_run(
        returnstack + 512, 
        datastack   + 1024,
        argc, 
        argv
    );

    printf("closing forth...\n");
    // free(reader_state);
    free(here0);

    return 0;
}
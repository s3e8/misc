#include "fth.h"
// #include <stdlib.h>
// #include <ctype.h>

#define DATASTACK_SIZE      1024
#define RETURNSTACK_SIZE    512

int main() 
{
    //* init *// // init forth first, cause it's the config for each //

    // init forth //            
    thread_state_t  thread;
    reader_state_t  reader;

    // thread init
    cell    datastack[DATASTACK_SIZE];
    void**  returnstack[RETURNSTACK_SIZE];
    thread.ds = datastack;
    thread.s0 = datastack;
    thread.rs = returnstack;
    thread.r0 = returnstack;

    // dict init
    here_size   = 10*1024*1024; // can it be resized?
    here0       = malloc(here_size); // should this even be malloc? ... no
    here        = here0;

    
    // reader init
    // reader_state_t  stdin_state;
    char            stdinbuf[1024];
    char wordbuf[WORD_NAME_MAX_LENGTH];
    char linebuf[WORD_NAME_MAX_LENGTH]; // todo: why the same len?
    // linebufsize
    // reader_open_file();

    // init interpreter
    // void **nestingstack_space[NESTINGSTACK_MAX_DEPTH];
    // void ***nestingstack = nestingstack_space + NESTINGSTACK_MAX_DEPTH;
    // void *builtin_immediatebuf[2] = { NULL, WORD(IRETURN) };
    // void *word_immediatebuf[3]    = { WORD(CALL), NULL, WORD(IRETURN) };
    // char stdinbuf[1024];

    init_reader_state(&reader, stdinbuf, sizeof(stdinbuf), stdin);

    // init forth vm
    forth_vm_t      forth;
    forth.thread = &thread;
    forth.reader = &reader;

    

    // init terminal stuff if not gui
    // init terminal loop


    // init audio loop
    // init game loop

    /* main loop */
    while (1) 
    {
        // gets atomic fth
        // fth_readline();

        printf("fth> ");
        fflush(stdout);

        char* word = read_word(&reader, wordbuf);
        if (word == NULL) break;

        if (strlen(word) > 0)
        {
            printf("Got word: %s\n", word);
            // fth_interpret_word(word, &thread);
        }
        // fgets(,, stdin);
        // interprets
        // sets
    }
}
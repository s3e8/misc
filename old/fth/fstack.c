#include <stdint.h>

#define STACK_SIZE 1024

typedef uintptr_t cell;


typedef struct thread_state_t {
    cell s0;
    cell r0;
    cell ds;
    cell rs;
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
}
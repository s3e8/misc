#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// dictionary stuff
#define WORD_NAME_MAX_LENGTH 32

#define BIT(x) (1<<(x)) // as opposed to (1<<x)??
#define FLAG_HIDDEN     BIT(0)
#define FLAG_IMMEDIATE  BIT(1)
#define FLAG_BUILTIN    BIT(2)
#define FLAG_HASARG     BIT(3)
#define FLAG_INLINE     BIT(4)
#define FLAG_DEFFERRED  BIT(5)

#define STATE_IMMEDIATE 0
#define STATE_COMPILE   1

/* the most important type, the cell. MUST be exactly of the pointer length! */
typedef uint_ptr_t cell;

typedef struct word_hdr_t {
    cell flags;

} word_hdr_t;

int main() {

    return 0; 
}
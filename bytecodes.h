#ifndef OPS

#define OP_EOW {}
#define OP_EXIT { ip = POPRS(); }
#define OP_LIT { PUSH(INTARG()); }
// #define OP_BRANCH {
//     tmp = INTARG();
//     ip += (tmp/sizeof(void*))-1;    
// }


BYTECODE(EOW,       "eow",      0, 0, 0,            OP_EOW)
BYTECODE(RETURN,    "exit",     0, 0, 0,            OP_EXIT)
BYTECODE(LIT,       "lit",      0, 0, FLAG_HASARG,  OP_LIT)
BYTECODE(INTERPRET, "interpret", 0, 0, 0, {
    char *word = read_word(inputstate,linebuf);
    if(!word) NEXT();
    dict_hdr_t *entry = find_word(word);
    if(!entry) {
        char *endptr = NULL;
        cell val = (cell)strtol(word, &endptr, base);
        if(*endptr!='\0') {
	        printf("ERROR: no such word: %s\n", word);
        } else {
            if(state==STATE_COMPILE) {
                comma((cell) &&l_LIT);
                comma(val);
            } else {
                PUSH(val);
            }
        }
        NEXT();
    }
    if(state==STATE_COMPILE && !(entry->flags & FLAG_IMMED)) {
        if(entry->flags & FLAG_BUILTIN) {
	        comma((cell)(*cfa(entry)));
        } else {
	        comma((cell) &&l_CALL);
	        comma((cell) cfa(entry));
        }
    } else {
        void **code = cfa(entry);
        *--nestingstack = ip;
        if(entry->flags & FLAG_BUILTIN) {
	        builtin_immediatebuf[0] = *code;
	        ip = builtin_immediatebuf;
        } else {
	        word_immediatebuf[1] = (void*)code;
	        ip = word_immediatebuf;
        }
    }
})
BYTECODE(BRANCH,    "branch",   0, 0, FLAG_HASARG,  {
    tmp = INTARG();
    ip += (tmp/sizeof(void*))-1;    
})
BYTECODE(CALL, "call", 0, 0, FLAG_HASARG, { 
    void *fn = ARG();
    PUSHRS(ip);
    ip = fn;    
})

#endif OPS
// #undef OPS
#include <stdio.h>

void write_file(const char* fname, const char* contents) {
    FILE* file = fopen(fname, "w");
    fprintf(file, contents);
}
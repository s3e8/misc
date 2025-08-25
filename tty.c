#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h> 

struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDERR_FILENO, &orig_termios);
    atexit(disable_raw_mode); // stdlib

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    tcsetattr(STDERR_FILENO, TCSAFLUSH, &raw);
}



int main() {
    enable_raw_mode();
    
    char c;
    while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        if (iscntrl(c)) printf("%d\r\n", c);
        else            printf("%d ('%c')\r\n", c, c);
    };

    disable_raw_mode();

    return 0;
}
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/
struct termios orig_termios;

/*** terminal ***/
void die(const char* s) {
    perror(s);
    exit(1);
}

void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        die("tcsetattr");
    };
}

void enable_raw_mode() {
    if (tcgetattr(STDERR_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disable_raw_mode); // stdlib

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char tty_read_key() {
    int nread;
    char c;
    while((nread = read(STDIN_FILENO, &c, 1) == -1)) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }

    return c;
}

/*** input ***/
void tty_process_key_press() {
    char c = tty_read_key();

    switch (c) {
        case CTRL_KEY('q'):
            exit(0);
            break;
        case 'q':
            exit(0);
            break;
    }
}



int main() {
  enable_raw_mode();

  while(1) {
    tty_process_key_press();
  }

  return 0;
}
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/
struct tty_cfg {
    int rows;
    int cols;
    struct termios orig_termios;
};

struct tty_cfg cfg;

/*** terminal ***/
void die(const char* s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &cfg.orig_termios) == -1) {
        die("tcsetattr");
    };
}

void enable_raw_mode() {
    if (tcgetattr(STDERR_FILENO, &cfg.orig_termios) == -1) die("tcgetattr");
    atexit(disable_raw_mode); // stdlib

    struct termios raw = cfg.orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |=  (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char tty_read_key() {
    int nread;
    char c;
    while((nread = read(STDIN_FILENO, &c, 1)) == -1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }

    return c;
}

int get_cursor_position(int* rows, int* cols) {
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

    while (i < sizeof(buf) - 1) {
        if(read(STDIN_FILENO, &buf[i], 1) == -1) break;
        if(buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

    // printf("\r\n&buf[1]: '%s'\r\n", &buf[1]);

    // printf("\r\n");
    // char c;
    // while (read(STDIN_FILENO, &c, 1) == 1) {
    //     if(iscntrl(c)) {
    //         printf("%d\r\n", c);
    //     }
    //     else {
    //         printf("%d ('%c')\r\n", c, c);
    //     }
    // }
    // tty_read_key();

    return -0;
}

int get_window_size(int* r, int* c) {
    struct winsize ws;

    if (1 || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        return get_cursor_position(r, c);
    } 
    else {
        *c = ws.ws_col;
        *r = ws.ws_row;
        return 0;
    }
}

/*** output ***/
void tty_draw_rows() {
    int y;
    for(y = 0; y < cfg.rows; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void tty_refresh_screen() {
    write(STDOUT_FILENO, "\x1b[2J]", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    tty_draw_rows();

    write(STDOUT_FILENO, "\x1b[H", 3);
}

/*** input ***/
void tty_process_key_press() {
    char c = tty_read_key();

    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}


/*** init ***/

void init_tty() {
    if (get_window_size(&cfg.rows, &cfg.cols) == -1) die("get_window_size");
}

int main() {
  enable_raw_mode();
  init_tty();

  while(1) {
    tty_refresh_screen();
    tty_process_key_press();
  }

  return 0;
}
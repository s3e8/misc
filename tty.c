#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <termios.h>


#define CTRL_KEY(k) ((k) & 0x1f)

struct tty_cfg
{
    int term_rows;
    int term_cols;
    struct termios termios_cfg;
};

struct tty_cfg tty;


/* terminal */
void tty_enable_raw_mode();
void tty_disable_raw_mode();
void tty_die(const char* s);
char tty_read_key();
int  tty_get_cursor_position(int* rows, int* cols);
int  tty_get_window_size(int* rows, int* cols);
void tty_process_keypress();
/* output */
void tty_draw_rows();
void tty_refresh_screen();
/* init */
void tty_init();



void tty_enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &tty.termios_cfg) == -1) tty_die("tcgetattr");
    atexit(tty_disable_raw_mode);

    struct termios raw = tty.termios_cfg;
    raw.c_iflag             /* input flags */
        &= ~(
            BRKINT  |       // old conventional stuff
            ICRNL   |       // CR = "carriage return", NL = "new line" 
            INPCK   |       // old conventional stuff
            ISTRIP  |       // old conventional stuff
            IXON            //
        );
    raw.c_oflag             /* output flags */
        &= ~(
            OPOST           // output post-processing
        );
    raw.c_cflag
        &= ~(
            CS8             // old conventional stuff
        );
    raw.c_lflag             /* local flags -or- "misc" flags */
        &= ~(
                ECHO    | 
                ICANON  |   // canonical -or- "todo:" mode
                IEXTEN  |   //
                ISIG        // 
            );
    /* control characters */
    raw.c_cc[VMIN]  = 0;    //
    raw.c_cc[VTIME] = 1;    // in tenths of a second

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) tty_die("tcsetattr");
}

void tty_disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tty.termios_cfg) == -1) tty_die("tcsetattr");
}

void tty_die(const char* s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H",  3);

    perror(s);
    write(STDOUT_FILENO, "\r",  1);
    exit(1);
}

char tty_read_key() {
    int  nread;
    char c; // todo: should we just pass this in from the global stack? maybe itll just be a forth thing later

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN) tty_die("read");
    }

    return c;
}

int tty_get_cursor_position(int* rows, int* cols) {
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

    return 0;
}

int tty_get_window_size(int* rows, int* cols) {
    struct winsize ws;

    // todo: when this errors "if (1 || ...)", the error message on die is "tty_get_window_size: Success" .. oh nvm, cause it's a fake failed test...
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) //  apparently == 0 a possible erroneous outcome
    {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        tty_get_cursor_position(rows, cols);

        return -1;
    }
    else 
    {
        *rows = ws.ws_row; // does this * dereference the pointer, so that it's changing the value and not the address itself?
        *cols = ws.ws_col;

        return 0;
    }
}

void tty_process_keypress() {
    char c = tty_read_key();

    switch (c)
    {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H",  3);
            exit(0);
            break;
    }
}

/* output */
void tty_draw_rows() {
    int y;
    for (y = 0; y < tty.term_rows; y++)
    {
        write(STDOUT_FILENO, "~", 1);

        if (y < tty.term_rows - 1) 
        {
            write(STDOUT_FILENO, "\r\n", 2);
        }
    }
}

void tty_refresh_screen() {
    write(STDOUT_FILENO, "\x1b[2J", 4); // 
    write(STDOUT_FILENO, "\x1b[H",  3); //

    tty_draw_rows();                    // 

    write(STDOUT_FILENO, "\x1b[H",  3); //
}

/* init */
void tty_init() {
    if (tty_get_window_size(&tty.term_rows, &tty.term_rows) == -1) tty_die("tty_get_window_size");
}

int main()
{
    tty_enable_raw_mode();
    tty_init();

    while (1)
    {
        tty_refresh_screen();
        tty_process_keypress();
    }
    // while (1) {
    //     char c = '\0';  // todo: do we have to initialize this every time?

    //     if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) tty_die("read");
    //     if (iscntrl(c))             printf("%d\r\n", c);
    //     else                        printf("%d ('%c')\r\n", c, c);
    //     // if (c == CTRL_KEY('q'))     break;
    //     if (c == 'q')               break; // temporary only for codespaces
    // }

    return 0;
}
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define TTY_VERSION "0.0.1"

#define CTRL_KEY(k) ((k) & 0x1f)

struct tty_cfg
{
    int cx; // horizontal   (x-axis)
    int cy; // vertical     (y-axis)
    int term_rows;
    int term_cols;
    struct termios termios_cfg;
};
struct tty_cfg tty;

/* append buffer */
struct abuf
{
    char* buf;
    int  len;
};
#define ABUF_INIT {NULL, 0}


/* terminal */
void tty_enable_raw_mode();
void tty_disable_raw_mode();
void tty_die(const char* s);
char tty_read_key();
int  tty_get_cursor_position(int* rows, int* cols);
int  tty_get_window_size(int* rows, int* cols);
/* append buffer */
void abuf_append(struct abuf* abuf, const char* s, int len);
void abuf_free(struct abuf* abuf);
/* output */
void tty_draw_rows(struct abuf* abuf);
void tty_refresh_screen();
/* input */
void tty_move_cursor(char key);
void tty_process_keypress();
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

/* append buffer */
void abuf_append(struct abuf* abuf, const char* s, int len) {
    char* new = realloc(abuf->buf, abuf->len + len);
    if (new == NULL) return;

    memcpy(&new[abuf->len], s, len);
    abuf->buf  = new;
    abuf->len += len;
}

void abuf_free(struct abuf* abuf) {
    free(abuf->buf);
}

/* output */
void tty_draw_rows(struct abuf* abuf) {
    int y;
    for (y = 0; y < tty.term_rows; y++)
    {
        if (y == tty.term_rows / 3)
        {
            char greet[80];
            int greet_length = snprintf(greet, sizeof(greet), "TTY editor -- version %s", TTY_VERSION);
            // "Press any button to continue..."
            // trkr options, etc

            if (greet_length > tty.term_cols) greet_length = tty.term_cols;

            int padding = (tty.term_cols - greet_length) / 2;
            if (padding)
            {
                abuf_append(abuf, "~", 1);
                padding--;
            }
            while (padding--) abuf_append(abuf, " ", 1);
            abuf_append(abuf, greet, greet_length);
        }
        else {
            abuf_append(abuf, "~", 1);
        }

        abuf_append(abuf, "\x1b[K", 3);      // clear one line
        if (y < tty.term_rows - 1) 
        {
            abuf_append(abuf, "\r\n", 2);
        }
    }
}

void tty_refresh_screen() {
    struct abuf abuf = ABUF_INIT;

    abuf_append(&abuf,  "\x1b[?25l",    6); // hide cursor
    // abuf_append(&abuf,  "\x1b[2J",   4); // clear entire screen
    abuf_append(&abuf,  "\x1b[H",       3); //
    tty_draw_rows(&abuf);                   // 

    // move the cursor to the position stored in tty.cx and tty.cy
    char cursor_buf[32];
    snprintf(cursor_buf, sizeof(cursor_buf), "\x1b[%d;%dH", tty.cy + 1, tty.cx + 1);
    abuf_append(&abuf, cursor_buf, strlen(cursor_buf));
    abuf_append(&abuf, "\x1b[?25h", 6);    // show cursor

    write(STDOUT_FILENO, abuf.buf, abuf.len);
    abuf_free(&abuf);
}

/* input */
void tty_move_cursor(char key) {
    switch (key)
    {
        case 'a':
            tty.cx--;
            break;
        case 'd':
            tty.cx++;
            break;
        case 'w':
            tty.cy--;
            break;
        case 's':
            tty.cy++;
            break;
    }
}

void tty_process_keypress() {
    char c = tty_read_key();

    switch (c)
    {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4); // what?
            write(STDOUT_FILENO, "\x1b[H",  3); // what?
            exit(0);
            break;
        
        case 'w':
        case 'a':
        case 's':
        case 'd':
            tty_move_cursor(c);
            break;
    }
}

/* init */
void tty_init() {
    tty.cx = 0;
    tty.cy = 0;

    if (tty_get_window_size(&tty.term_rows, &tty.term_cols) == -1) tty_die("tty_get_window_size");
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

    return 0;
}
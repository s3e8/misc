#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios termios_cfg;

void tty_enable_raw_mode();
void tty_disable_raw_mode();
void tty_die(const char* s);

void tty_enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &termios_cfg) == -1) tty_die("tcgetattr");
    atexit(tty_disable_raw_mode);

    struct termios raw = termios_cfg;
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
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_cfg) == -1) tty_die("tcsetattr");
}

void tty_die(const char* s) {
    perror(s);
    exit(1);
}

int main()
{
    tty_enable_raw_mode();

    while (1)
    {
        char c = '\0';  // todo: do we have to initialize this every time?

        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) tty_die("read");
        if (iscntrl(c)) printf("%d\r\n", c);
        else            printf("%d ('%c')\r\n", c, c);
        if (c == 'q')   break;
    }

    return 0;
}
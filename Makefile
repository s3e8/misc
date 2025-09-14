tty: tty.c
	$(CC) tty.c -o ./bin/tty -Wall -Wextra -pedantic -std=c99

trkr: trkr.c
	$(CC) trkr.c -o ./bin/trkr -Wall -Wextra -pedantic -std=c99
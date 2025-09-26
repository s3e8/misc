## DIR stuff ##
# $(BUILD_DIR) = ./bin

## C stuff ##
# $(CC) = cc
# $(CFLAGS) = -Wall -Wextra -pedantic -std=c99


tty: tty.c
	$(CC) 	tty.c 	-o 	./bin/tty 	-Wall -Wextra -pedantic -std=c99

fth: fth.c
	$(CC)	fth.c 	-o	./bin/fth	-Wall -Wextra -pedantic -std=c99

trkr: trkr.c
	$(CC) 	trkr.c 	-o 	./bin/trkr 	-Wall -Wextra -pedantic -std=c99
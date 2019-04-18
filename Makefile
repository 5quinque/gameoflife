CC=gcc
FLAGS=-Wall -Wextra -Wshadow -std=c99 -lm -D_POSIX_C_SOURCE=199309L
LIBS=-lncurses
BIN=/usr/bin/

gameoflife: ./src/main.c
	${CC} ./src/main.c ${FLAGS} ${LIBS} -o ./bin/gameoflife

clean:
	rm ./bin/gameoflife


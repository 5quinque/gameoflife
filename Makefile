CC=gcc
FLAGS=-Wall -Wextra -Wshadow -std=c99 -lm -D_POSIX_C_SOURCE=199309L
LIBS=-lncurses
BIN=/usr/bin/

gameoflife: main.c
	${CC} main.c ${FLAGS} ${LIBS} -o gameoflife

clean:
	rm gameoflife


CC = gcc
FLAGS = -Wall -Wextra -Werror -g3 -O0
# -g -O0 allows gdb debugging
# -Werror converts all warnings to errors

all: clean grive2daemon 

clean:
	rm -f grive2daemon

grive2daemon: grive2daemon.c
	${CC} ${FLAGS} grive2daemon.c -o grive2daemon
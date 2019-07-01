CC = gcc
FLAGS = -Wall -Wextra -Werror -g3 -O0
# -g -O0 allows gdb debugging
# -Werror converts all warnings to errors

all: clean daemon 

clean:
	rm -f daemon

daemon: daemon.c
	${CC} ${FLAGS} daemon.c -o daemon

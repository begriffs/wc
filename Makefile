CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -O3 -Wall -Wextra -Wpedantic -Wshadow -Werror

all : simple threads lessSimple

threads: threads.c
	cc $(CFLAGS) -o threads $? -pthread

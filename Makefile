CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -O3 -Wall -Wextra -Wpedantic -Wshadow -Werror

all : simple-getchar simple-read threads

threads: threads.c
	cc $(CFLAGS) -o $@ $? -pthread

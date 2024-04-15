CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic

all: proj2

proj2: proj2.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f your_target_name


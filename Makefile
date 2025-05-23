# Makefile for proj2
# Author: Timotej Mikula, xmikult00
# Date: 28.4.2024

CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -lrt
LDFLAGS = -pthread -lrt
OUTPUT = proj2

build: proj2.c proj2.h
	$(CC) $(CFLAGS) proj2.c -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

zip:
	zip proj2.zip proj2.c proj2.h Makefile

CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread

proj2: proj2.c
	@$(CC) $(CFLAGS) -o proj2 proj2.c

clean:
	@rm proj2

zip:
	zip proj2 proj2.c Makefile

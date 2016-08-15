.PHONY: all

all: server client

%: %.c chat.c
	gcc $< chat.c -o $@ -std=gnu99 -Wall -Werror

clean:
	rm -rf server client

valgrind: server client
	valgrind --leak-check=yes ./server

test: valgrind
	ipcs -m



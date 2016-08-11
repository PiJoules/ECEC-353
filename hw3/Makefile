.PHONY: all

all: server

%: %.c
	gcc $< chat.c -o $@ -std=gnu99 -Wall -Werror

clean:
	rm -rf server client



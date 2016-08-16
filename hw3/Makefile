.PHONY: all

EXE = server client test_linked_list
LIB = chat.c hash.c list.c

all: $(EXE)


%: %.c $(LIB)
	gcc $< $(LIB) -o $@ -std=gnu99 -Wall -Werror

clean:
	rm -rf $(EXE)

valgrind: $(EXE)
	valgrind --leak-check=yes ./test_linked_list

test: valgrind
	ipcs -m



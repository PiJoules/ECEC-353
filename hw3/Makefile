.PHONY: all

EXE = server client
LIB = chat.c hashtable.c linked_list.c

all: $(EXE)


%: %.c $(LIB)
	gcc $< $(LIB) -o $@ -std=gnu99 -Wall -Werror -pthread

clean:
	rm -rf $(EXE)

valgrind: $(EXE)
	#valgrind --leak-check=yes ./test_linked_list

test: valgrind
	ipcs -m



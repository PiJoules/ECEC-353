# Usage

Dynamic loading
```sh
$ gcc *.c -std=c99 -o mini_grep # compile; may need to add -pthread if the pthread library isn't automatically linked
$ ./mini_grep system . 4 dynamic
```

There should be 12 occurances:
- 7 in mini_grep.c
- 1 in this README.md
- 2 in the test_sub_dir/test.txt
- 2 in the compiled binary mini_grep executable

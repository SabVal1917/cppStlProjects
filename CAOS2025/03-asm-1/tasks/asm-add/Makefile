all: test_simple

simple: test_simple/test_simple.c add.s
	gcc -z noexecstack test_simple/test_simple.c add.s -o $@

test_simple: input_simple/input_simple.txt simple
	./simple

.PHONY: all test_simple

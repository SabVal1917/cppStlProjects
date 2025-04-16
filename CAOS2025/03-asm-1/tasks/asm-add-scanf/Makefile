all: test_simple

simple: test_simple/test_simple.c add_scanf.s
	gcc -z noexecstack -no-pie test_simple/test_simple.c add_scanf.s -o $@

test_simple: input_simple/input_simple_scan.txt input_simple/input_simple.txt simple
	./simple

.PHONY: all test_simple

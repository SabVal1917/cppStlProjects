all: test_simple

simple: test_simple/test_simple.c add_scanf.s
	aarch64-linux-gnu-gcc test_simple/test_simple.c add_scanf.s -o $@

test_simple: input_simple/input_simple.txt simple
	qemu-aarch64 -L /usr/aarch64-linux-gnu ./simple

.PHONY: all test_simple

all: test_simple

simple: test_simple/test_simple.c longest_inc_subseq.s
	aarch64-linux-gnu-gcc test_simple/test_simple.c longest_inc_subseq.s -o $@

test_simple: input_simple/input.txt input_simple/input_answers.txt simple
	qemu-aarch64 -L /usr/aarch64-linux-gnu ./simple

.PHONY: all test_simple

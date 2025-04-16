all: test_tailf

tailf: tailf.c Makefile
	gcc tailf.c -I. -Wall -Werror -O3 -fno-omit-frame-pointer -o $@

test_tailf: tailf
	timeout 1s python3 -u test_tailf.py ./tailf

.PHONY: all test_tailf

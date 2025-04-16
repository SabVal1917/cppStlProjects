run: test_utf8
	python3 test.py ./test_utf8

test_utf8: main.c utf8_file.c utf8_file.h Makefile
	gcc main.c utf8_file.c -I. -Wall -Werror -O3 -fno-omit-frame-pointer -o $@

.PHONY: run

all: test_minigrep

minigrep: minigrep.c Makefile
	gcc minigrep.c -I. -I/opt/homebrew/Cellar/pcre/8.45/include/ -lpcre -Wall -Werror -O3 -fno-omit-frame-pointer -fsanitize=address -o $@

test_minigrep: minigrep
	python3 -u test_minigrep.py ./minigrep

.PHONY: all test_minigrep

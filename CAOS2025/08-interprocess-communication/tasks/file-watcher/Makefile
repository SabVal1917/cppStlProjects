run: test_file_watcher
	python3 test.py ./test_file_watcher

test_file_watcher: main.c Makefile
	gcc main.c -I. -Wall -Werror -O3 -fno-omit-frame-pointer -o $@

.PHONY: run

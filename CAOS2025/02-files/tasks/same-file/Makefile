run: same_file
	python3 ./test.py ./same_file

same_file: Makefile main.c
	gcc -Wall -Werror main.c -o $@

.PHONY: run

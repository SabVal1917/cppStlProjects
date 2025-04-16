run: kv_server
	python3 test.py ./kv_server

kv_server: main.c Makefile
	gcc main.c -I. -Wall -Werror -O3 -fno-omit-frame-pointer -o $@

.PHONY: run

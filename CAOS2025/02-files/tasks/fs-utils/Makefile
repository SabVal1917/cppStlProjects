all: test_rm test_mkdir

test_rm: rm
	python3 test_rm.py ./rm

rm: rm.c Makefile
	gcc rm.c -I. -Wall -Werror -O3 -fno-omit-frame-pointer -o $@

test_mkdir: mkdir
	python3 test_mkdir.py ./mkdir

mkdir: mkdir.c Makefile
	gcc mkdir.c -I. -Wall -Werror -O3 -fno-omit-frame-pointer -o $@

.PHONY: all test_rm test_mkdir

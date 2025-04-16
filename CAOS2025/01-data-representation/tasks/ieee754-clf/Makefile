default: run

test_ieee754_clf: main.c ieee754_clf.c ieee754_clf.h Makefile
	gcc main.c ieee754_clf.c -Wall -Werror -fsanitize=undefined -I. -O3 -fno-omit-frame-pointer -o $@

run: test_ieee754_clf
	./test_ieee754_clf

.PHONY: run

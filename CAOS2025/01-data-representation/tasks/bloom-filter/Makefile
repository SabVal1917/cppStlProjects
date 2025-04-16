default: run

tester: tester.c bloom_filter.c bloom_filter.h
	gcc tester.c bloom_filter.c -I. -O3 -fsanitize=undefined -o tester

run: tester
	./tester

.PHONY: run

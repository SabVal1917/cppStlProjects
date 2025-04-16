all: run

tester:
	gcc -z noexecstack main.c dot_product.s -o main

run: tester
	python3 test.py

.PHONY: run

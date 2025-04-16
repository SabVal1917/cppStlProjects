all: simple_tests

simple_tests:
	echo "===== simple_tests =====" && \
	gcc -z noexecstack simple_tests/test.c memcpy.s -o main && \
	./main && \
	rm main

.PHONY: all simple_tests

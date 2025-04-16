default: run

test: test.c integral.h Makefile
	gcc test.c -g -I. -O3 -fsanitize=undefined -o test -lpthread -lm

test_asan: test.c integral.h Makefile
	gcc test.c -g -I. -DASAN -O3 -fsanitize=address -o test_asan -lpthread -lm

test_tsan: test.c integral.h Makefile
	gcc test.c -g -I. -DTSAN -O3 -fsanitize=thread -o test_tsan -lpthread -lm

run: test test_asan test_tsan
	echo "Testing with no sanitizers (almost)..."
	./test
	echo "Testing with Address Sanitizer..."
	./test_asan
	echo "Testing with Thread Sanitizer..."
	TSAN_OPTIONS="report_signal_unsafe=0" ./test_tsan
	echo "All public tests PASSED"

.PHONY: run

default: run

test: test.c lockfree-stack.h
	gcc test.c -g -I. -O3 -fsanitize=undefined -o test -lpthread -latomic

test_asan: test.c lockfree-stack.h
	gcc test.c -g -I. -DASAN -O3 -fsanitize=address -o test_asan -lpthread -latomic

test_tsan: test.c lockfree-stack.h
	gcc test.c -g -I. -DTSAN -O3 -fsanitize=thread -o test_tsan -lpthread -latomic

run_public: test test_asan test_tsan
	echo "Testing with no sanitizers (almost)..."
	./test
	echo "Testing with Address Sanitizer..."
	./test_asan
	echo "Testing with Thread Sanitizer..."
	TSAN_OPTIONS="report_signal_unsafe=0" ./test_tsan
	echo "All tests PASSED"

include ./private.make # remove for local run

run: run_public run_private

.PHONY: run

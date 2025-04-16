default: run

tester: tester.c falloc.h falloc.c
	gcc tester.c falloc.c -I. -O3 -fsanitize=undefined -o tester

run: tester
	rm -f storage

	# ===== test_forbidden_words ================
	cat falloc.c | grep -cE "(m|c|re)alloc" > count || true
	cat count | xargs test 0 -eq

	cat falloc.c | grep -cE "MAP_ANONYMOUS" > count || true
	cat count | xargs test 0 -eq

	cat falloc.c | grep -cE "brk" > count || true
	cat count | xargs test 0 -eq
	# ===========================================

	# ===== test_munmap =========================
	cat falloc.c | grep -cE "munmap" > count || true
	cat count | xargs test 1 -le
	# ===========================================

	./tester
	
	rm -f tester storage count

.PHONY: run

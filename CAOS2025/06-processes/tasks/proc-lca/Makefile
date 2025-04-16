default: run

tester: tester.c lca.h lca.c
	gcc tester.c lca.c -I. -O3 -fsanitize=undefined -o tester

run: tester
	# ===== test_forbidden_words ================
	cat lca.c | grep -cE "system" > count || true
	cat count | xargs test 0 -eq
	# ===========================================

	./tester
	
	rm -f tester count

.PHONY: run

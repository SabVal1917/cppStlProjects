default: run

chainy: chainy.c
	gcc chainy.c -O3 -fsanitize=undefined -o chainy

run: chainy
	# ===== test_forbidden_words ================
	cat chainy.c | grep -cE "system" > count || true
	cat count | xargs test 0 -eq

	cat chainy.c | grep -cE "F_SETPIPE_SZ" > count || true
	cat count | xargs test 0 -eq
	# ===========================================

	# ===== test_pipe ===========================
	cat chainy.c | grep -cE "pipe" > count || true
	cat count | xargs test 1 -le
	# ===========================================

	python3 tester.py

	rm -rf count chainy content

.PHONY: run

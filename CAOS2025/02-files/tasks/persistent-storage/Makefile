default: run

tester: tester.c storage.h storage.c
	gcc tester.c storage.c -I. -O3 -fsanitize=undefined -o tester

run: tester
	rm -rf unit_root persistence_root stress_root

	mkdir unit_root
	mkdir persistence_root
	mkdir stress_root

	./tester unit
	./tester persistence before_crash
	./tester persistence after_crash
	./tester stress

	# ===== test_many_keys =================
	ls -ld ./stress_root | wc -l > count
	cat count | xargs test 100 -ge
	# ======================================

	# ===== test_many_values ===============
	ls -ld ./stress_root/ca | wc -l > count
	cat count | xargs test 1 -ge
	# ======================================
	
	rm -rf unit_root persistence_root stress_root count tester

.PHONY: run

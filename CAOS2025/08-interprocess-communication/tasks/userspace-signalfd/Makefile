run: signalfd
	./signalfd

signalfd: main.c signalfd.c
	gcc main.c signalfd.c -I. -Werror -Wall -O3 -fno-omit-frame-pointer -o signalfd

.PHONY: run

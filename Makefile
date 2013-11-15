CC=gcc
CFLAGS=-g -Wall -Werror -I./lib/src
LDLIBS=-lm

.PHONY: clean clear

renderer: src/renderer.c lib/openrt.a
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

lib/openrt.a:
	$(MAKE) -C lib openrt.a

clean:
	$(MAKE) -C lib clean
	find . -name "*~" -delete

clear: clean
	$(MAKE) -C lib clear
	rm -f renderer

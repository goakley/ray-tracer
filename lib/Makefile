CC=gcc
CFLAGS=-g -Wall -Werror
LDLIBS=-lm

.PHONY: clean clear

openrt.a: obj/vec3f.o obj/shapes.o obj/openrt.o
	ar -cvr $@ $^

obj/openrt.o: src/openrt.c
	$(CC) $(CFLAGS) $^ -c -o $@ $(LDLIBS)

obj/vec3f.o: src/vec3f.c
	$(CC) $(CFLAGS) $^ -c -o $@ $(LDLIBS)

obj/shapes.o : src/shapes.c
	$(CC) $(CFLAGS) $^ -c -o $@ $(LDLIBS)

clean:
	find . -name "*~" -delete

clear: clean
	rm -f openrt.a
	rm -f obj/*.o

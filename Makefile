all: pi

pi: pi.c
	cc pi.c -std=c99 -o pi

run: pi
	./pi
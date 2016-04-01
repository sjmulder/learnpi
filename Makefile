all: pi

pi: pi.c
	cc pi.c -std=c99 -Wall -lm -o pi

run: pi
	./pi
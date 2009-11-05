all: pi

pi: pi.c
	cc pi.c -std=c99 -Wall -o pi

run: pi
	./pi
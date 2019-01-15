#makefile for count.c

CC=gcc
CFLAGS = -g -Wall

all: count

count: lab1.c
	$(CC) $(CFLAGS) -o count lab1.c

clean:
	rm count

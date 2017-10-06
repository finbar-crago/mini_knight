CC=clang
SIZE ?= 8
CFLAGS=-ansi -pedantic -Wall -g -O3 -DSIZE=$(SIZE)

all: knight

clean:
	rm -f knight

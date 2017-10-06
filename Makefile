CC=clang
SIZE ?= 8
CFLAGS=-ansi -pedantic -Wall -g -DSIZE=$(SIZE)
all: knight

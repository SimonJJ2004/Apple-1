SDIR=src
BDIR=bin

SRC=$(SDIR)/main.cpp

BIN=$(BDIR)/apple1

CC=g++
SDL=`sdl2-config --cflags --libs`

main:
	$(CC) $(SRC) -o $(BIN) $(SDL)


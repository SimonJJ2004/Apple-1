SDIR=src
BDIR=bin

SRC=$(SDIR)/main.cpp

BIN=$(BDIR)/apple1

ROMS=roms/*

CC=g++
SDL=`sdl2-config --cflags --libs`

main:
	$(CC) $(SRC) -o $(BIN) $(SDL)

install:
	mkdir -p /usr/local/bin
	cp $(BIN) /usr/local/bin/apple1
	mkdir -p ~/Apple-1/roms 
	cp $(ROMS) ~/Apple-1/roms

remove:
	rm -r ~/Apple-1
	rm /usr/local/bin/apple1
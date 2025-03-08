SDIR=src
BDIR=bin

SRC=$(SDIR)/*.cpp

BIN=$(BDIR)/apple1

ROMS=roms/*

CC=g++
SDL=`sdl2-config --cflags --libs`

main:
	$(CC) $(SRC) -o $(BIN) $(SDL) -std=c++17

install:
	mkdir -p /usr/local/bin
	cp $(BIN) /usr/local/bin/apple1
	mkdir -p /etc/Apple-1/roms 
	cp $(ROMS) /etc/Apple-1/roms

remove:
	rm -r /etc/Apple-1
	rm /usr/local/bin/apple1
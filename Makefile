#==========================================
#    Makefile: makefile for sl 5.1
#	Copyright 1993, 1998, 2014
#                 Toyoda Masashi
#		  (mtoyoda@acm.org)
#	Last Modified: 2014/03/31
#==========================================

CC=gcc
CFLAGS=-O -Wall

all: sl

sl: sl.c 
	$(CC) $(CFLAGS) -o sl sl.c -lncurses
	cp sl ./usr/bin/sl
	cp @*.txt ./usr/share/ascii_art

clean:
	rm -f sl
	rm -f ./usr/bin/sl
	rm -f ./usr/share/ascii_art/*

distclean: clean

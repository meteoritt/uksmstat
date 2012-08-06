DESTDIR?=
PREFIX?=/usr/local
CC=gcc
CFLAGS=-O3 -std=c99 -W -Wall -pedantic -D_GNU_SOURCE
all: uksmstat
uksmstat: uksmstat.o
	$(CC) $(CFLAGS) -lc uksmstat.o -o $@
uksmstat.o: uksmstat.c
	$(CC) $(CFLAGS) -c uksmstat.c -o $@
install:
	install -Dm 0755 uksmstat $(DESTDIR)$(PREFIX)/bin/uksmstat
clean:
	rm -f uksmstat.o
	rm -f uksmstat

# Makefile for gemdrop-cc65 tools
# Bill Kendrick
# 2015-07-03 - 2015-07-03

all:	fonts-to-h

fonts-to-h:	fonts-to-h.o
	$(CC) $(CFLAGS) $(LDFLAGS) fonts-to-h.o -o fonts-to-h

fonts-to-h.o:	fonts-to-h.c
	$(CC) $(CFLAGS) fonts-to-h.c -c -o fonts-to-h.o


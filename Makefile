CC=gcc
CFLAGS=-I.

simul.fs: FileManager.c
	$(CC) -w -o simulfs FileManager.c -I .

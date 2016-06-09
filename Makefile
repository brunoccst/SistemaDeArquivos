CC=gcc
CFLAGS=-I.

simul.fs: FileManager.c
	$(CC) -o simulfs FileManager.c -I.

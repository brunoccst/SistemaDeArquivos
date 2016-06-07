CC=gcc
CFLAGS=-I.

simul.fs: FileManager.c
	$(CC) -o simul.fs FileManager.c -I.

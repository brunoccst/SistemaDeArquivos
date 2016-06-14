CC = gcc
FILES = FileManager.c
OUT_EXE = simulfs

build: $(FILES)
	$(CC) -w -o $(OUT_EXE) $(FILES)

clean:
	rm -f simulfs

rebuild: clean build

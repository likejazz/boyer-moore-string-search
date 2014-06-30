CC=gcc
CFLAGS=-Wno-pointer-sign

build: boyer-moore.c
	$(CC) -o bm boyer-moore.c $(CFLAGS)

clean:
	rm -rf bm

CC=gcc
CFLAGS=-g -Wall
TARGET=lox

all:
	$(CC) $(CFLAGS) src/utils.c src/main.c -o $(TARGET)

clean:
	rm -f lox *.o a.out && rm -rf lox.dSym




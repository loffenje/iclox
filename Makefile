CC=gcc
CFLAGS=-g -Wall
TARGET=lox

all:
	$(CC) $(CFLAGS) src/memory.c src/utils.c src/token.c src/parser.c src/expr.c src/scanner.c src/main.c -o $(TARGET)

clean:
	rm -f lox *.o a.out && rm -rf lox.dSym




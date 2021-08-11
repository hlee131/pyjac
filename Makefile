GCC=gcc

all:
	$(GCC) -Wall -std=c99 src/compiler.c src/lexer.c src/ast.c

debug:
	$(GCC) -g src/compiler.c src/lexer.c -o debug.out

clean:
	rm -f a.out debug.out

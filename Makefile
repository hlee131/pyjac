GCC=gcc

all:
	$(GCC) -Wall -std=c99 src/compiler.c src/lexer.c src/ast.c src/parser.c src/list.c src/utils.c

debug:
	$(GCC) -Wall -g src/compiler.c src/lexer.c src/ast.c src/parser.c src/list.c src/utils.c -o debug.out

clean:
	rm -f a.out debug.out

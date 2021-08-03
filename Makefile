GCC=gcc

all:
	$(GCC) -Wall src/compiler.c src/lexer.c

debug:
	$(GCC) -g src/compiler.c src/lexer.c -o debug.out

clean:
	rm -f a.out debug.out

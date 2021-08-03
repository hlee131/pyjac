#include <stdio.h> 
#include <stdlib.h> 

#include "includes/lexer.h"
#include "includes/compiler.h" 

int main(int argc, char* argv[]) {
	
	if (argc < 2) {
		puts("error: please supply a file name");
		return 1; 
	}

	char* file = read_file(argv[1]); 
	Lexer* lex = init_lexer(file);

	do { next(lex); } while (lex->curr_tok.tok_type != NULL_TOK);
	return 0; 
}

char* read_file(char* file_name) {
	FILE* source = fopen(file_name, "r");
	fseek(source, 0L, SEEK_END);
	long size = ftell(source);
	fseek(source, 0L, SEEK_SET); 
	char* file = calloc(size, sizeof(char));
	fread(file, sizeof(char), size, source);
	fclose(source);
	return file; 
}

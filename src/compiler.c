#include <stdlib.h>

#include "lexer.c"

int main(int argc, char* argv[]) {

	char* file = read_file(argv[1]); 
	Lexer* 


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

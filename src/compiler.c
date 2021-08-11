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

	if (file) {	
		token_stream_t* stream = make_stream(file);
		for (int i = 0; i < stream->stream_len; i++) {
			print_token(stream->stream[i]); 
		}
		return 0; 
	} else {
		puts("error: invalid file name");
		return 1; 
	}
}

char* read_file(char* file_name) {
	FILE* source = fopen(file_name, "r");
	
	if (source) {
		fseek(source, 0L, SEEK_END);
		long size = ftell(source);
		fseek(source, 0L, SEEK_SET); 
		char* file = calloc(size, sizeof(char));
		fread(file, sizeof(char), size, source);
		fclose(source);
		return file; 
	} else return NULL; 
}

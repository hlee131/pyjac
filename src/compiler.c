#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include <llvm-c/Analysis.h> 
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/BitWriter.h> 

#include "includes/parser.h"
#include "includes/compiler.h" 
#include "includes/utils.h"
#include "includes/symtab.h"
#include "includes/codegen.h"

int main(int argc, char* argv[]) {

	// settings
	char* source_file; 
	char* out_file = "out";
	int keep_flag = 0;  
	
	// parse non option argument 
	source_file = argv[1]; 
	argc--; argv++; 

	// parse flags 
	while (*argv) {
		keep_flag |= strcmp(*argv, "--keep-files") == 0;
		if (strcmp(*argv, "-o") == 0) {
			if (++argv) out_file = *argv;
			else puts("-o missing file name"); 
		}
		argv++; 
	}

	char* file = read_file(source_file);
	if (file) {	
		parser_t* parser = init_parser(file);
		parse_program(parser);
		bool types = do_type_check(parser->ast);
		if (types) {
			puts("types incorrect");
		} else {
			puts("types correct");
		}
		
		// generate executable 
		char* error = NULL;
		LLVMModuleRef app = generate_module(parser->ast); 
		LLVMDumpModule(app);
		LLVMVerifyModule(app, LLVMAbortProcessAction, &error);
		LLVMDisposeMessage(error); 
		if (LLVMWriteBitcodeToFile(app, ".tmp.bc")) {
			puts("error writing bitcode, aborting compilation");
			abort(); 
		} else {
			// generate builtins object for linking 
			system("clang -c src/builtins.c -o .built.o"); 
			// ir to assembly 
			system("llc -filetype=obj .tmp.bc"); 
			// assembly to machine code 
			char command[14 + strlen(source_file) + 1];
			strcpy(command, "gcc .tmp.o .built.o -o "); 
			strcat(command, out_file);
			system(command); 
			// clean up
			if (!keep_flag) system("rm -f .tmp.bc .tmp.o .built.o");
			return 0; 
		}
	} 
	puts("error: invalid file name");
	return 1; 
}


char* read_file(char* file_name) {
	FILE* source = fopen(file_name, "r");
	
	if (source) {
		fseek(source, 0L, SEEK_END);
		long size = ftell(source);
		fseek(source, 0L, SEEK_SET); 
		char* file = checked_malloc(size * sizeof(char));
		fread(file, sizeof(char), size, source);
		fclose(source);
		return file; 
	} else return NULL; 
}
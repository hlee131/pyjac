#include <stdio.h> 
#include <stdlib.h> 

#include <llvm-c/Analysis.h> 
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/BitWriter.h> 

#include "includes/parser.h"
#include "includes/compiler.h" 
#include "includes/utils.h"
#include "includes/symtab.h"
#include "includes/codegen.h"

int main(int argc, char* argv[]) {
	
	if (argc < 2) {
		puts("error: please supply a file name");
		return 1; 
	}

	char* file = read_file(argv[1]);

	if (file) {	
		parser_t* parser = init_parser(file);
		// token_stream_t* s = parser->token_stream; 
		// for (int i = 0; i < s->stream_len; i++) {
		// 	puts(tok_string(s->stream[i].tok_type)); 
		// }
		// printf("%d\n", parser->token_stream->stream_len);
		parse_program(parser);
		bool types = do_type_check(parser->ast);
		if (types) {
			puts("types incorrect");
		} else {
			puts("types correct");
		}
		
		// jit driver 
		// generate code and get reference to entry point, i.e. main function 
		char* error = NULL;
		LLVMModuleRef app = generate_module(parser->ast); 
		if (LLVMWriteBitcodeToFile(app, "pcc.bc")) {
			puts("error writing bitcode");
		}
		LLVMVerifyModule(app, LLVMAbortProcessAction, &error);
		LLVMDisposeMessage(error); 
		LLVMValueRef entry_point = LLVMGetNamedFunction(app, "main");

		// create the execution engine 
		LLVMExecutionEngineRef engine; 
		error = NULL;
		LLVMLinkInMCJIT();
		LLVMInitializeNativeTarget(); 
		LLVMInitializeNativeAsmPrinter(); 
		if (LLVMCreateExecutionEngineForModule(&engine, app, &error) != 0) {
			printf("cannot create execution engine, aborting ... \n");
			abort(); 
		}
		if (error) {
			printf("error: %s", error);
			LLVMDisposeMessage(error);
			exit(EXIT_FAILURE); 
		}

		// get function pointer 
		int (*prog_main)() = (int (*)()) LLVMGetFunctionAddress(engine, "main");
		prog_main(); 

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
		char* file = checked_malloc(size * sizeof(char));
		fread(file, sizeof(char), size, source);
		fclose(source);
		return file; 
	} else return NULL; 
}

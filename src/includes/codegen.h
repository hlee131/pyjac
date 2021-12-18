#ifndef CODEGEN_H
#define CODEGEN_H

#include <llvm-c/Core.h> 
#include "ast.h"

// macro to create LLVM types
LLVMModuleRef generate_module(prog_ast_t program);
LLVMTypeRef get_LLVM_type(type_node_t* type);


#endif 

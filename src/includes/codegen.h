#ifndef CODEGEN_H
#define CODEGEN_H

#include <llvm-c/Core.h> 
#include "ast.h"
#include "symtab.h"

LLVMModuleRef generate_module(prog_ast_t program);
LLVMTypeRef get_LLVM_type(type_node_t* type);
LLVMValueRef alloca_at_entry(struct symtab_s* ref_table, LLVMTypeRef type, char* name, LLVMBuilderRef builder); 
symtab_t* make_llvm_symtab(prog_ast_t program, LLVMModuleRef mod);
LLVMBasicBlockRef generate_bb(LLVMBuilderRef builder, block_ast_t block, symtab_t* ref_table);
LLVMValueRef generate_expr(LLVMBuilderRef builder, expr_ast_t* expr, symtab_t* ref_table);
LLVMValueRef generate_binop(LLVMBuilderRef builder, expr_ast_t* expr, symtab_t* ref_table);
void write_state(LLVMBuilderRef builder, state_ast_t* state, symtab_t* ref_table);
LLVMTypeRef get_LLVM_type(type_node_t* type); 

#endif 

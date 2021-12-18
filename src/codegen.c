#include "includes/codegen.h"

LLVMModuleRef generate_module(prog_ast_t program) {
    // make new module 
    LLVMModuleRef app = LLVMModuleCreateWithName("app"); 

    // add each function to module 
    foreach (program, curr_func) {
        state_ast_t* function = (state_ast_t*) curr_func->current_ele;
        LLVMTypeRef param_types[function->children.func.params->length]; 
        
        int i = 0; 
        foreach (function->children.func.params, curr_p) {
            id_ast_t* param = (id_ast_t*) curr_p->current_ele; 
            param_types[i++] = get_LLVM_type(param->id_type); 
        }

        LLVMTypeRef func_type = LLVMFunctionType(
            get_LLVM_type(function->children.func.identifier->id_type), 
            param_types,
            function->children.func.params->length,
            0); 

        LLVMValueRef func = LLVMAddFunction(app, 
            function->children.func.identifier->name, 
            func_type); 
    }
}

// turn ast leaves into basic block 
LLVMBasicBlockRef generate_bb(block_ast_t block) {

}

// write expressions to module 
void write_expressions(LLVMBuilderRef builder, expr_ast_t* expr) {

}

// write comparisons, loop constructs, etc. to module 
void write_state(LLVMBuilderRef builder, state_ast_t* state) {
    
}

// convert ast types into llvm types 
// precondition: no error subtypes, i.e. abort compilation if 
// errors in type checking 
// TODO: figure out how to find length of arrays and strings 
LLVMTypeRef get_LLVM_type(type_node_t* type) {
    // if (type->arr_count != 0) {
    //     type->arr_count--;
    //     return LLVMArrayType(get_LLVM_type(type), ); 
    // }
    
    switch (type->type) {
        case BOOL_T: 
        case INT_T:     
            return LLVMInt32Type();
        case DOUBLE_T:  return LLVMDoubleType();
        // case STR_T:
        //     return LLVMArrayType(LLVMInt32Type(), )
        
    }
}
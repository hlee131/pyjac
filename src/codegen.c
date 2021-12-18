#include "includes/codegen.h"

LLVMModuleRef generate_module(prog_ast_t program) {
    // make new module 
    LLVMModuleRef app = LLVMModuleCreateWithName("app");
    LLVMBuilderRef builder = LLVMCreateBuilder();  

    // add each function to module 
    foreach (program, curr_func) {
        // generate array of param types 
        state_ast_t* function = (state_ast_t*) curr_func->current_ele;
        LLVMTypeRef param_types[function->children.func.params->length]; 
        struct symtab_s* global_symtab = make_global_symtab(program); 
        
        int i = 0; 
        foreach (function->children.func.params, curr_p) {
            id_ast_t* param = (id_ast_t*) curr_p->current_ele; 
            param_types[i++] = get_LLVM_type(param->id_type); 
        }

        // generate function signature 
        LLVMTypeRef func_type = LLVMFunctionType(
            get_LLVM_type(function->children.func.identifier->id_type), 
            param_types,
            function->children.func.params->length,
            0); 

        // add function to module 
        LLVMValueRef func = LLVMAddFunction(app, 
            function->children.func.identifier->name, 
            func_type); 

        LLVMBasicBlockRef function_body = LLVMAppendBasicBlock(app, "body");
        LLVMPositionBuilderAtEnd(builder, function_body); 
        generate_bb(builder, function->children.func.block); 
        // TODO: alloca arguments so that arguments are mutable 
    }
}

// turn ast leaves into basic block 
LLVMBasicBlockRef generate_bb(LLVMBuilderRef builder, block_ast_t block) {
    foreach (block, curr) {
        write_state(builder, curr->current_ele); 
    }
}

// build value references from expressions 
LLVMValueRef generate_expr(LLVMBuilderRef builder, expr_ast_t* expr) {
    switch (expr->kind) {
        case BINOP:
            generate_binop(builder, &(expr->children.binop)); 
            break; 
        case CALL:
        case INT_L:
        case BOOL_L: 
            return LLVMConstInt(LLVMInt32Type(), expr->children.int_val, 0); 
        case DOUBLE_L:
            return LLVMConstReal(LLVMDoubleType(), expr->children.double_val); 
        case STR_L:
        case ID_L:
    }
}

LLVMValueRef generate_binop(LLVMBuilderRef builder, struct binop_ast* binop) {
    switch (binop->op) {
        case ADD_NODE:
            LLVMBuildFadd(builder, 
                generate_expr(builder, binop->lhs), 
                generate_expr(builder, binop->rhs), "add"); 
            break; 
        case SUB_NODE:
        case MUL_NODE:
        case DIV_NODE:
        case EQ_NODE:
        case NEQ_NODE:
        case LT_NODE:
        case GT_NODE:
        case LE_NODE:
        case GE_NODE:
        case INDEX_NODE:
        case ASSIGN_NODE: 

    }
}

// write comparisons, loop constructs, etc. to module 
void write_state(LLVMBuilderRef builder, state_ast_t* state) {
    switch (state->kind) {
        case IF:
            // store current if and else block 
            foreach (state->children.if_tree, curr) {
                if_pair_t* pair = curr->current_ele; 
                //    
            }
        case FOR:
        case WHILE:
        case RET:
            LLVMBuildRet(builder, generate_expr(builder, state->children.ret.expression)); 
            break; 
        case ASSIGN:
            // allocate local variable on stack 
            // TODO: position builder in function entry block 
            LLVMValueRef addr = LLVMBuildAlloca(builder, 
                get_LLVM_type(state->children.assign.identifier->id_type),
                state->children.assign.identifier->name); 
            // store value in local variable 
            LLVMBuildStore(builder, generate_expr(builder, state->children.assign.value), addr); 
            // add to symtab 

        case EXPR: 
    }
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
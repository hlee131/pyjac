#include "includes/codegen.h"
#include "includes/symtab.h"

LLVMModuleRef generate_module(prog_ast_t program) {
    // make new module 
    LLVMModuleRef app = LLVMModuleCreateWithName("app");
    LLVMBuilderRef builder = LLVMCreateBuilder();  
    struct symtab_s* ref_table = make_llvm_symtab(program, app); 

    // add each function to module 
    foreach (program, curr_func) {
        enter_scope(ref_table); 

        state_ast_t* function = (state_ast_t*) curr_func->current_ele;
        LLVMValueRef func_ref = 
            lookup(ref_table, function->children.func.identifier->name)->type.val_ref;

        LLVMBasicBlockRef function_body = LLVMAppendBasicBlock(func_ref, "body");

        ref_table->entry_block = LLVMGetEntryBasicBlock(func_ref); 
        
        // allocate parameters on stack for mutability 
        int param_count = 0; 
        foreach (function->children.func.params, curr) {
            id_ast_t* param = (id_ast_t*) curr->current_ele; 
            LLVMValueRef addr = alloca_at_entry(func_ref, get_LLVM_type(param->id_type), param->name, builder); 
            LLVMBuildStore(builder, LLVMGetParam(func_ref, param_count++), addr); 

            // store addr into ref table 
            insert_LLVM_ref(addr, VAR_SYM, param->name, ref_table); 
        }

        LLVMPositionBuilderAtEnd(builder, function_body); 
        generate_bb(builder, function->children.func.block, ref_table); 

        exit_scope(ref_table); 
    }
}

LLVMValueRef alloca_at_entry(struct symtab_s* ref_table, LLVMTypeRef type, char* name, LLVMBuilderRef builder) {
    LLVMBasicBlockRef entry_block = ref_table->entry_block; 
    LLVMPositionBuilderAtEnd(builder, entry_block); 
    LLVMBuildAlloca(builder, type, name); 
}

struct symtab_s* make_llvm_symtab(prog_ast_t program, LLVMModuleRef mod) {
    struct symtab_s* table = init_symtab(); 
    foreach (program, curr_func) {
        // generate array of param types 
        state_ast_t* function = (state_ast_t*) curr_func->current_ele;
        LLVMTypeRef param_types[function->children.func.params->length]; 
        
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
        LLVMValueRef func = LLVMAddFunction(mod, 
            function->children.func.identifier->name, 
            func_type); 

        // insert ref into symbol table
        insert_LLVM_ref(func, FUNC_SYM, function->children.func.identifier->name, table); 
    }
    return table;
}

// turn ast leaves into basic block 
LLVMBasicBlockRef generate_bb(LLVMBuilderRef builder, block_ast_t block, symtab_t* ref_table) {
    foreach (block, curr) {
        write_state(builder, curr->current_ele, ref_table); 
    }
}

// build value references from expressions 
LLVMValueRef generate_expr(LLVMBuilderRef builder, expr_ast_t* expr, symtab_t* ref_table) {
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
            // load from stack 
            LLVMValueRef addr = lookup(ref_table, expr->children.str_val);
            return LLVMBuildLoad2(builder, LLVMTypeOf(addr), addr, expr->children.str_val); 
    }
}

LLVMValueRef generate_binop(LLVMBuilderRef builder, struct binop_ast* binop) {
    switch (binop->op) {
        case ADD_NODE:
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
void write_state(LLVMBuilderRef builder, state_ast_t* state, symtab_t* ref_table) {
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
            LLVMBuildRet(builder, generate_expr(builder, state->children.ret.expression, ref_table)); 
            break; 
        case ASSIGN:
            // allocate local variable on stack 
            LLVMValueRef addr = alloca_at_entry(ref_table, 
                get_LLVM_type(state->children.assign.identifier->id_type), 
                state->children.assign.identifier->name, builder); 
            // store value in local variable 
            LLVMBuildStore(builder, generate_expr(builder, state->children.assign.value, ref_table), addr); 
            // add to symtab 
            insert_LLVM_ref(addr, VAR_SYM, state->children.assign.identifier->name, ref_table); 
            break; 
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
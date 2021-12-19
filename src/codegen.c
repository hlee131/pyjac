#include "includes/codegen.h"
#include "includes/symtab.h"
#include "includes/ast.h"
#include "includes/list.h"

LLVMModuleRef generate_module(prog_ast_t program) {
    // make new module 
    LLVMModuleRef app = LLVMModuleCreateWithName("app");
    LLVMBuilderRef builder = LLVMCreateBuilder();  
    symtab_t* ref_table = make_llvm_symtab(program, app); 

    // add each function to module 
    foreach (program, curr_func) {
        enter_scope(ref_table); 

        // locate function reference in symbol table 
        // and append a basic block as the function body 
        state_ast_t* function = (state_ast_t*) curr_func->current_ele;
        LLVMValueRef func_ref = 
            lookup(ref_table, function->children.func.identifier->name)->type.val_ref;

        LLVMBasicBlockRef function_body = LLVMAppendBasicBlock(func_ref, "body");

        // store entry block for variable allocations and 
        // for getting the parent function reference 
        ref_table->entry_block = LLVMGetEntryBasicBlock(func_ref); 
        
        // allocate parameters on stack for mutability 
        int param_count = 0; 
        foreach (function->children.func.params, curr) {
            id_ast_t* param = (id_ast_t*) curr->current_ele; 
            // allocate on stack and build store instruction 
            LLVMValueRef addr = alloca_at_entry(ref_table, get_LLVM_type(param->id_type), param->name, builder); 
            LLVMBuildStore(builder, LLVMGetParam(func_ref, param_count++), addr); 

            // store addr into ref table 
            insert_LLVM_ref(addr, VAR_SYM, param->name, ref_table); 
        }

        // generate function body basic block 
        LLVMPositionBuilderAtEnd(builder, function_body); 
        generate_bb(builder, function->children.func.block, ref_table); 

        // cleanup: exit scope, check if return void is needed 
        // unallocate arguments and local variables 
        exit_scope(ref_table); 
    }
}

// allocate variables in entry block because memory to register optimizations only 
// scan the function's entry block 
LLVMValueRef alloca_at_entry(struct symtab_s* ref_table, LLVMTypeRef type, char* name, LLVMBuilderRef builder) {
    LLVMPositionBuilderAtEnd(builder, ref_table->entry_block); 
    LLVMBuildAlloca(builder, type, name); 
}

symtab_t* make_llvm_symtab(prog_ast_t program, LLVMModuleRef mod) {
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
        // ensure that the next instruction is positioned correctly, e.g.
        // position builder in the merge branch if it is a control flow structure 
        LLVMPositionBuilderAtEnd(builder, LLVMGetLastBasicBlock(
            LLVMGetBasicBlockParent(ref_table->entry_block))); 
    }
}

// build value references from expressions 
LLVMValueRef generate_expr(LLVMBuilderRef builder, expr_ast_t* expr, symtab_t* ref_table) {
    switch (expr->kind) {
        case BINOP:
            return generate_binop(builder, expr, ref_table); 
        // TODO: implement function call 
        case CALL:
        case INT_L:
        case BOOL_L: 
            return LLVMConstInt(LLVMInt32Type(), expr->children.int_val, 0); 
        case DOUBLE_L:
            return LLVMConstReal(LLVMDoubleType(), expr->children.double_val); 
        case STR_L:
        case ID_L: {
            // load from stack using addr from symbol table 
            LLVMValueRef addr = lookup(ref_table, expr->children.str_val)->type.val_ref;
            return LLVMBuildLoad2(builder, LLVMTypeOf(addr), addr, expr->children.str_val); 
        }
    }
}

// TODO: make cleaner with arrays? 
LLVMValueRef generate_binop(LLVMBuilderRef builder, expr_ast_t* expr, symtab_t* ref_table) {
    LLVMValueRef lhs = generate_expr(builder, expr->children.binop.lhs, ref_table);
    LLVMValueRef rhs = generate_expr(builder, expr->children.binop.rhs, ref_table);
    switch (expr->children.binop.op) {
        case ADD_NODE:
            if (expr->expr_type->type == INT_T) return LLVMBuildAdd(builder, lhs, rhs, "int_add"); 
            else return LLVMBuildFAdd(builder, lhs, rhs, "double_add"); 
        case SUB_NODE:
            if (expr->expr_type->type == INT_T) return LLVMBuildSub(builder, lhs, rhs, "int_sub");
            else return LLVMBuildFSub(builder, lhs, rhs, "double_sub"); 
        case MUL_NODE:
            if (expr->expr_type->type == INT_T) return LLVMBuildMul(builder, lhs, rhs, "int_mul");
            else return LLVMBuildFMul(builder, lhs, rhs, "double_mul"); 
        case DIV_NODE:
            // TODO: once negatives are supported, check if int is signed 
            if (expr->expr_type->type == INT_T) return LLVMBuildUDiv(builder, lhs, rhs, "int_div");
            else return LLVMBuildExactUDiv(builder, lhs, rhs, "double_div"); 
        case EQ_NODE:
            if (expr->expr_type->type == INT_T) return LLVMBuildICmp(builder, LLVMIntEQ, lhs, rhs, "int_eq"); 
            else return LLVMBuildFCmp(builder, LLVMRealUEQ, lhs, rhs, "double_eq"); 
        case NEQ_NODE:
            if (expr->expr_type->type == INT_T) return LLVMBuildICmp(builder, LLVMIntNE, lhs, rhs, "int_neq"); 
            else return LLVMBuildFCmp(builder, LLVMRealUNE, lhs, rhs, "double_neq");
        case LT_NODE:
            if (expr->expr_type->type == INT_T) return LLVMBuildICmp(builder, LLVMIntULT, lhs, rhs, "int_lt"); 
            else return LLVMBuildFCmp(builder, LLVMRealULT, lhs, rhs, "double_lt");
        case GT_NODE:
            if (expr->expr_type->type == INT_T) return LLVMBuildICmp(builder, LLVMIntUGT, lhs, rhs, "int_gt"); 
            else return LLVMBuildFCmp(builder, LLVMRealUGT, lhs, rhs, "double_gt");
        case LE_NODE:
            if (expr->expr_type->type == INT_T) return LLVMBuildICmp(builder, LLVMIntULE, lhs, rhs, "int_le"); 
            else return LLVMBuildFCmp(builder, LLVMRealULE, lhs, rhs, "double_le");
        case GE_NODE:
            if (expr->expr_type->type == INT_T) return LLVMBuildICmp(builder, LLVMIntUGE, lhs, rhs, "int_ge"); 
            else return LLVMBuildFCmp(builder, LLVMRealUGE, lhs, rhs, "double_ge");
        case INDEX_NODE:
        case ASSIGN_NODE: 
            return LLVMBuildStore(builder, rhs, lookup(ref_table, expr->children.str_val)->type.val_ref); 
    }
}

// write comparisons, loop constructs, etc. to module 
void write_state(LLVMBuilderRef builder, state_ast_t* state, symtab_t* ref_table) {
    // TODO: condition checks needs to have own basic block to loop back to 
    switch (state->kind) {
        case IF: {
            LLVMBasicBlockRef if_head = LLVMGetLastBasicBlock(LLVMGetBasicBlockParent(ref_table->entry_block)); 
            LLVMBasicBlockRef curr_bb = if_head; 
            LLVMBasicBlockRef merge_bb = LLVMCreateBasicBlockInContext(LLVMGetGlobalContext(), "merge-branch"); 
            foreach (state->children.if_tree, curr) {
                if_pair_t* pair = curr->current_ele; 
                LLVMValueRef condition = generate_expr(builder, pair->condition, ref_table);
                enter_scope(ref_table); 
                LLVMBasicBlockRef t = generate_bb(builder, pair->block, ref_table); 
                exit_scope(ref_table); 
                LLVMPositionBuilderAtEnd(builder, t);
                LLVMBuildBr(builder, merge_bb); 
                LLVMBasicBlockRef f = LLVMCreateBasicBlockInContext(LLVMGetGlobalContext(), "else"); 
                LLVMBuildCondBr(builder, condition, t, f); 
                // keeps track where to insert the next conditional or 
                // merge branch 
                curr_bb = f; 
            }
            // merge into the next statement by creating an empty basic block the next statement adds to 
            LLVMPositionBuilderAtEnd(builder, curr_bb);
            LLVMBuildBr(builder, merge_bb); 
            break; 
        }
        case FOR: {
            enter_scope(ref_table); 
            // create merge basic block for when the for loop ends 
            LLVMBasicBlockRef merge_bb = LLVMCreateBasicBlockInContext(LLVMGetGlobalContext(), "merge-branch"); 
            // write the initializer 
            write_state(builder, state->children.for_tree.initializer, ref_table); 

            // create condition in new basic block 
            LLVMBasicBlockRef for_head = LLVMAppendBasicBlock(LLVMGetBasicBlockParent(ref_table->entry_block), "for-head"); 
            LLVMBuildBr(builder, for_head); 
            LLVMPositionBuilderAtEnd(builder, for_head); 
            LLVMValueRef condition = generate_expr(builder, state->children.for_tree.condition, ref_table); 
            LLVMBasicBlockRef for_block = generate_bb(builder, state->children.for_tree.block, ref_table); 

            // write conditional branch 
            LLVMBuildCondBr(builder, condition, for_block, merge_bb); 

            // write updater into the end of the block 
            // and branch back into the conditional 
            LLVMPositionBuilderAtEnd(builder, for_block);
            generate_expr(builder, state->children.for_tree.updater, ref_table); 
            LLVMBuildBr(builder, for_head);

            // exit for loop scope 
            exit_scope(ref_table); 
            break; 
        }
        case WHILE: {
            enter_scope(ref_table); 
            LLVMBasicBlockRef merge_bb = LLVMCreateBasicBlockInContext(LLVMGetGlobalContext(), "merge-branch");

            // put condition in new basic block 
            LLVMBasicBlockRef while_head = LLVMAppendBasicBlock(LLVMGetBasicBlockParent(ref_table->entry_block), "while-head");
            LLVMBuildBr(builder, while_head); 
            LLVMPositionBuilderAtEnd(builder, while_head); 
            LLVMValueRef condition = generate_expr(builder, state->children.while_tree.condition, ref_table);
            LLVMBasicBlockRef while_bb = generate_bb(builder, state->children.while_tree.block, ref_table); 
            LLVMBuildCondBr(builder, condition, while_bb, merge_bb); 

            // loop back to condition 
            LLVMPositionBuilderAtEnd(builder, while_bb);
            LLVMBuildBr(builder, while_head); 
            
            exit_scope(ref_table);
            break; 
        }
        case RET:
            LLVMBuildRet(builder, generate_expr(builder, state->children.ret.expression, ref_table)); 
            break; 
        case ASSIGN: {
            // allocate local variable on stack 
            LLVMValueRef addr = alloca_at_entry(ref_table, 
                get_LLVM_type(state->children.assign.identifier->id_type), 
                state->children.assign.identifier->name, builder); 
            // store value in local variable 
            LLVMBuildStore(builder, generate_expr(builder, state->children.assign.value, ref_table), addr); 
            // add to symtab 
            insert_LLVM_ref(addr, VAR_SYM, state->children.assign.identifier->name, ref_table); 
            break; 
        }
        case EXPR: 
            generate_expr(builder, &(state->children.expr), ref_table); 
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
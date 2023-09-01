#include <stdio.h>
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
        LLVMValueRef func_ref 		= 
			ref_table->curr_func 	= 
            lookup(ref_table, function->children.func.identifier->name)->type.val_ref;
        
		LLVMBasicBlockRef entry = LLVMAppendBasicBlock(func_ref, "function-body"); 
		LLVMPositionBuilderAtEnd(builder, entry); 

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
		foreach (function->children.func.block, curr) {
			write_state(builder, curr->current_ele, ref_table); 
			if (RET_CHECK(LLVMGetInsertBlock(builder))) break; 
		}

        // cleanup: exit scope, check if return void is needed 
        // unallocate arguments and local variables 
        exit_scope(ref_table); 
    }

	return app; 
}

// allocate variables in entry block because memory to register optimizations only 
// scan the function's entry block 
LLVMValueRef alloca_at_entry(struct symtab_s* ref_table, LLVMTypeRef type, char* name, LLVMBuilderRef builder) {
    LLVMBasicBlockRef old_position = LLVMGetInsertBlock(builder);
    LLVMBasicBlockRef entry_bb = LLVMGetEntryBasicBlock(ref_table->curr_func);
    LLVMPositionBuilder(builder, entry_bb, LLVMGetFirstInstruction(entry_bb)); 
    LLVMValueRef return_val = LLVMBuildAlloca(builder, type, name); 
    LLVMPositionBuilderAtEnd(builder, old_position); 
    return return_val;
}

symtab_t* make_llvm_symtab(prog_ast_t program, LLVMModuleRef mod) {
    struct symtab_s* table = init_symtab(); 
	// insert user defined functions 
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

	// insert builtin functions 
	LLVMTypeRef show_args[] = { LLVMInt32Type() }; 
	LLVMValueRef show = LLVMAddFunction(mod, "show", LLVMFunctionType(
		LLVMInt32Type(), show_args, 1, 0 
	)); 
	LLVMTypeRef min_max_args[] = { LLVMInt32Type(), LLVMInt32Type() }; 
	LLVMValueRef max = LLVMAddFunction(mod, "max", LLVMFunctionType(
		LLVMInt32Type(), min_max_args, 2, 0
	));
	LLVMValueRef min = LLVMAddFunction(mod, "min", LLVMFunctionType(
		LLVMInt32Type(), min_max_args, 2, 0
	)); 

	insert_LLVM_ref(show, FUNC_SYM, "show", table); 
	insert_LLVM_ref(max, FUNC_SYM, "max", table);
	insert_LLVM_ref(min, FUNC_SYM, "min", table);

    return table;
}

// turn ast subtrees into basic block 
LLVMBasicBlockRef generate_bb(LLVMBuilderRef builder, block_ast_t block, symtab_t* ref_table) {
	LLVMBasicBlockRef new_bb = LLVMAppendBasicBlock(ref_table->curr_func, "bb"); 
	LLVMPositionBuilderAtEnd(builder, new_bb);
    foreach (block, curr) {
        write_state(builder, curr->current_ele, ref_table); 
		if (RET_CHECK(LLVMGetInsertBlock(builder))) return new_bb; 
    }
	return new_bb; 
}

// build value references from expressions 
LLVMValueRef generate_expr(LLVMBuilderRef builder, expr_ast_t* expr, symtab_t* ref_table) {
    switch (expr->kind) {
        case BINOP:
            return generate_binop(builder, expr, ref_table); 
        case CALL: {
			char* func_name = expr->children.call.func_name; 
			LLVMValueRef func_ref = lookup(ref_table, func_name)->type.val_ref;
			LLVMTypeRef func_type = LLVMGetReturnType(LLVMTypeOf(func_ref));
			unsigned int argc = expr->children.call.params->length; 
			LLVMValueRef args[argc]; 

			// generate array of arguments 
			int i = 0;
			foreach(expr->children.call.params, curr) 
				args[i++] = generate_expr(builder, curr->current_ele, ref_table); 
			
			return LLVMBuildCall2(builder, func_type, func_ref, args, argc, func_name); 
		}
        case INT_L:
        case BOOL_L: 
            return LLVMConstInt(LLVMInt32Type(), expr->children.int_val, 0); 
        case DOUBLE_L:
            return LLVMConstReal(LLVMDoubleType(), expr->children.double_val); 
        case STR_L:
        case ID_L: {
            // load from stack using addr from symbol table 
            LLVMValueRef addr = lookup(ref_table, expr->children.str_val)->type.val_ref;
            return LLVMBuildLoad2(builder, LLVMGetAllocatedType(addr), addr, expr->children.str_val); 
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
            if (expr->children.binop.lhs->expr_type->type == INT_T) 
				return LLVMBuildICmp(builder, LLVMIntEQ, lhs, rhs, "int_eq"); 
            else return LLVMBuildFCmp(builder, LLVMRealUEQ, lhs, rhs, "double_eq"); 
        case NEQ_NODE:
            if (expr->children.binop.lhs->expr_type->type == INT_T) 
				return LLVMBuildICmp(builder, LLVMIntNE, lhs, rhs, "int_neq"); 
            else return LLVMBuildFCmp(builder, LLVMRealUNE, lhs, rhs, "double_neq");
        case LT_NODE:
            if (expr->children.binop.lhs->expr_type->type == INT_T) 
				return LLVMBuildICmp(builder, LLVMIntULT, lhs, rhs, "int_lt"); 
            else return LLVMBuildFCmp(builder, LLVMRealULT, lhs, rhs, "double_lt");
        case GT_NODE:
            if (expr->children.binop.lhs->expr_type->type == INT_T) 
				return LLVMBuildICmp(builder, LLVMIntUGT, lhs, rhs, "int_gt"); 
            else return LLVMBuildFCmp(builder, LLVMRealUGT, lhs, rhs, "double_gt");
        case LE_NODE:
            if (expr->children.binop.lhs->expr_type->type == INT_T) 
				return LLVMBuildICmp(builder, LLVMIntULE, lhs, rhs, "int_le"); 
            else return LLVMBuildFCmp(builder, LLVMRealULE, lhs, rhs, "double_le");
        case GE_NODE:
            if (expr->children.binop.lhs->expr_type->type == INT_T) 
				return LLVMBuildICmp(builder, LLVMIntUGE, lhs, rhs, "int_ge"); 
            else return LLVMBuildFCmp(builder, LLVMRealUGE, lhs, rhs, "double_ge");
        case INDEX_NODE:
        case ASSIGN_NODE: 
			// TODO: remove extraneous load in optimizations?
			LLVMInstructionEraseFromParent(lhs); 
            return LLVMBuildStore(builder, rhs, 
				lookup(ref_table, expr->children.binop.lhs->children.str_val)->type.val_ref); 
    }
}

// write comparisons, loop constructs, etc. to module 
void write_state(LLVMBuilderRef builder, state_ast_t* state, symtab_t* ref_table) {
    switch (state->kind) {
        case IF: {
			LLVMBasicBlockRef curr_bb = LLVMGetInsertBlock(builder); 
            // LLVMBasicBlockRef merge_bb = LLVMAppendBasicBlock(ref_table->curr_func, "merge-if");
			LLVMBasicBlockRef merge_bb = NULL; 
            foreach (state->children.if_tree, curr) {
				if_pair_t* pair = curr->current_ele; 
				LLVMPositionBuilderAtEnd(builder, curr_bb); 
				// else blocks have a null condition 
				if (!pair->condition) {
					foreach (pair->block, curr) 
						write_state(builder, curr->current_ele, ref_table); 
				} else {
					LLVMValueRef condition = generate_expr(builder, pair->condition, ref_table);
					enter_scope(ref_table); 
					LLVMBasicBlockRef t = generate_bb(builder, pair->block, ref_table); 
					exit_scope(ref_table); 
					LLVMBasicBlockRef f = LLVMAppendBasicBlock(ref_table->curr_func, "else"); 
					LLVMPositionBuilderAtEnd(builder, curr_bb); 
					LLVMBuildCondBr(builder, condition, t, f); 
					// keeps track where to insert the next conditional or merge branch 
					curr_bb = f; 
					// check if merge needed 
					if (!RET_CHECK(t)) {
						if (!merge_bb) merge_bb = LLVMAppendBasicBlock(ref_table->curr_func, "merge-if");  
						LLVMPositionBuilderAtEnd(builder, t);
						LLVMBuildBr(builder, merge_bb); 
					}
				}
            }
            // check if merge needed 
			if (!RET_CHECK(curr_bb)) {
				if (!merge_bb) merge_bb = LLVMAppendBasicBlock(ref_table->curr_func, "merge-if");
				LLVMPositionBuilderAtEnd(builder, curr_bb);
            	LLVMBuildBr(builder, merge_bb); 
			}
			if (merge_bb) LLVMPositionBuilderAtEnd(builder, merge_bb);
            return; 
        }
        case FOR: {
            enter_scope(ref_table); 
            // create merge basic block for when the for loop ends 
            LLVMBasicBlockRef merge_bb = LLVMAppendBasicBlock(ref_table->curr_func, "merge-for"); 
            // write the initializer 
            write_state(builder, state->children.for_tree.initializer, ref_table); 

            // create condition in new basic block 
            LLVMBasicBlockRef for_head = LLVMAppendBasicBlock(ref_table->curr_func, "for-head"); 
            LLVMBuildBr(builder, for_head); 
            LLVMPositionBuilderAtEnd(builder, for_head); 
            LLVMValueRef condition = generate_expr(builder, state->children.for_tree.condition, ref_table); 
            LLVMBasicBlockRef for_block = generate_bb(builder, state->children.for_tree.block, ref_table); 
			// generate update and branch in leaf nodes 
			generate_expr(builder, state->children.for_tree.updater, ref_table);
			LLVMBuildBr(builder, for_head);
            // write conditional branch 
			LLVMPositionBuilderAtEnd(builder, for_head); 
            LLVMBuildCondBr(builder, condition, for_block, merge_bb); 

            // exit for loop scope 
            exit_scope(ref_table); 
			LLVMPositionBuilderAtEnd(builder, merge_bb);
            return; 
        }
        case WHILE: {
            enter_scope(ref_table); 
            LLVMBasicBlockRef merge_bb = LLVMAppendBasicBlock(ref_table->curr_func, "merge-while");

            // put condition in new basic block 
            LLVMBasicBlockRef while_head = LLVMAppendBasicBlock(ref_table->curr_func, "while-head");
            LLVMBuildBr(builder, while_head); 
            LLVMPositionBuilderAtEnd(builder, while_head); 
            LLVMValueRef condition = generate_expr(builder, state->children.while_tree.condition, ref_table);
            LLVMBasicBlockRef while_bb = generate_bb(builder, state->children.while_tree.block, ref_table); 
			// loop back to condition in leaf nodes
            LLVMBuildBr(builder, while_head); 
			LLVMPositionBuilderAtEnd(builder, while_head); 
            LLVMBuildCondBr(builder, condition, while_bb, merge_bb); 
            
            exit_scope(ref_table);
			LLVMPositionBuilderAtEnd(builder, merge_bb);
            return; 
        }
        case RET:
            LLVMBuildRet(builder, generate_expr(builder, state->children.ret.expression, ref_table)); 
            return; 
        case ASSIGN: {
            // allocate local variable on stack 
            LLVMValueRef addr = alloca_at_entry(ref_table, 
                get_LLVM_type(state->children.assign.identifier->id_type), 
                state->children.assign.identifier->name, builder); 
            // store value in local variable 
            LLVMBuildStore(builder, generate_expr(builder, state->children.assign.value, ref_table), addr); 
            // add to symtab 
            insert_LLVM_ref(addr, VAR_SYM, state->children.assign.identifier->name, ref_table); 
            return; 
        }
        case EXPR: 
            generate_expr(builder, &(state->children.expr), ref_table); return; 
    }
}

// convert ast types into llvm types 
// precondition: no error subtypes, i.e. abort compilation if 
// errors in type checking 
// TODO: figure out how to find length of arrays and strings 
// then add argc and argv back into main
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
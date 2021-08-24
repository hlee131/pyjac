#include <stdlib.h>
#include <stdbool.h>

#include "includes/ast.h"
#include "includes/list.h"
#include "includes/utils.h"
#include "includes/symtab.h"

// constructors for expressions 
expr_ast_t* int_node(int val, int line, int pos) {
	expr_ast_t* node = checked_malloc(sizeof(expr_ast_t));
	node->kind = INT_L;
	node->line = line;
	node->pos = pos; 
	node->children.int_val = val;
	return node; 
}

expr_ast_t* str_node(char* value, int line, int pos) {
	expr_ast_t* node = checked_malloc(sizeof(expr_ast_t)); 
	node->kind = STR_L;
	node->line = line;
	node->pos = pos;
	node->children.str_val = value; 
	return node; 
}

expr_ast_t* double_node(double val, int line, int pos) {
	expr_ast_t* node = checked_malloc(sizeof(expr_ast_t));
	node->kind = DOUBLE_L;
	node->line = line;
	node->pos = pos; 
	node->children.double_val = val;
	return node; 
}

expr_ast_t* id_node(char* value, int line, int pos) {
	expr_ast_t* node = str_node(value, line, pos); 
	node->kind = ID_L;
	return node; 
}

expr_ast_t* bool_node(int val, int line, int pos) {
	expr_ast_t* node = int_node(val, line, pos);
	node->kind = BOOL_L;
	return node; 
}

expr_ast_t* call_ast(char* func, list_t* params, int line, int pos) {
	expr_ast_t* ast = checked_malloc(sizeof(expr_ast_t));
	ast->line = line;
	ast->pos = pos;
	ast->kind = CALL;
	ast->children.call = (struct call_ast) {
		.func_name = func, .params = params 
	};
	return ast; 
}

expr_ast_t* binop_ast(int op, expr_ast_t* lhs, expr_ast_t* rhs, int line, int pos) {
	expr_ast_t* ast = checked_malloc(sizeof(expr_ast_t)); 
	ast->line = line;
	ast->pos = pos;
	ast->kind = BINOP; 
	ast->children.binop = (struct binop_ast) {
		.rhs = rhs, .lhs = lhs, .op = op
	};
	return ast; 
}

// constructors for statements
state_ast_t* if_ast(list_t* if_pairs, int line, int pos) { 
	state_ast_t* ast = checked_malloc(sizeof(state_ast_t));
	ast->kind = IF;
	ast->line = line; 
	ast->pos = pos;
	ast->children.if_tree.if_pairs = if_pairs; 
	return ast; 
}

state_ast_t* for_ast(state_ast_t* initializer, expr_ast_t* condition, expr_ast_t* updater, list_t* block, int line, int pos) {
	state_ast_t* ast = checked_malloc(sizeof(state_ast_t)); 
	ast->line = line;
	ast->pos = pos;
	ast->kind = FOR; 
	ast->children.for_tree = (struct for_ast) {
		.initializer = initializer,
		.condition = condition,
		.updater = updater,
		.block = block 
	}; 
	return ast; 
}

state_ast_t* while_ast(expr_ast_t* condition, list_t* block, int line, int pos) {
	state_ast_t* ast = checked_malloc(sizeof(state_ast_t)); 
	ast->children.while_tree = (struct while_ast) {
		.condition = condition,
		.block = block
	};
	ast->kind = WHILE; 
	ast->line = line;
	ast->pos = pos; 
	return ast; 
}

state_ast_t* func_ast(id_ast_t* identifier, list_t* params, list_t* block, int line, int pos) {
	state_ast_t* ast = checked_malloc(sizeof(state_ast_t));
	ast->children.func = (struct func_ast) {
		.identifier = identifier,
		.params = params,
		.block = block
	};
	ast->kind = FUNC; 
	ast->line = line;
	ast->pos = pos; 
	return ast; 
}

state_ast_t* ret_ast(expr_ast_t* expr, int line, int pos) {
	state_ast_t* ast = checked_malloc(sizeof(state_ast_t)); 
	ast->children.ret.expression = expr;
	ast->line = line;
	ast->kind = RET; 
	ast->pos = pos;
	return ast; 
}

state_ast_t* decl_ast(id_ast_t* id, expr_ast_t* val, int line, int pos) {
	state_ast_t* ast = checked_malloc(sizeof(state_ast_t));
	ast->children.assign = (struct decl_ast) {
		.identifier = id,
		.value = val
	};
	ast->line = line;
	ast->pos = pos; 
	ast->kind = ASSIGN; 
	return ast; 
}

state_ast_t* expr_ast(expr_ast_t* expr, int line, int pos) {
	state_ast_t* ast = checked_malloc(sizeof(state_ast_t));
	ast->children.expr = *expr; 
	ast->line = line;
	ast->pos = pos;
	return ast; 
}

// misc ast nodes and trees
if_pair_t* if_pair(expr_ast_t* condition, list_t* block) {
	if_pair_t* pair = checked_malloc(sizeof(if_pair_t)); 
	pair->condition = condition;
	pair->block = block; 
	return pair; 
}

type_node_t* type_node(int type, int arr_count) {
	type_node_t* node = checked_malloc(sizeof(type_node_t));
	node->type = type;
	node->arr_count = arr_count; 
	return node; 
}

id_ast_t* id_ast(char* name, type_node_t* id_type) {
	id_ast_t* ast = checked_malloc(sizeof(id_ast_t));
	ast->name = name;
	ast->id_type = id_type;
	return ast; 
}

// methods to perform semantic analysis on ast
/* 
Purpose: the driver function to type check a block of code
Return type: a boolean representing whether type checing 
	succeeded or failed 
*/ 
bool type_check(symtab_t* type_env, list_t* program) {
	// TODO: manage symbol table

}

/* 
Purpose: type checks a statement
Return type: a boolean representing whether type checking 
	succeeded or failed 
*/ 
bool type_check_state(symtab_t* type_env, state_ast_t* statement) {
	bool success = true; 
	
	switch (statement->kind) {
		case IF: 
			list_el_t* next = statement->children.if_tree.if_pairs->head;
			while (next) {
				list_el_t* curr = next;
				next = curr->next; 
				type_node_t* expr_type = type_check_expr(type_env, 
					((if_pair_t*) curr->current_ele)->condition);
				if (expr_type && expr_type->type == BOOL_T && expr_type->arr_count == 0) {
					free(expr_type);
					success &= type_check(type_env, 
						((if_pair_t*) curr->current_ele)->block);
				} else {
					// TODO: error message
				}
			}
			break; 
		case FOR:
			if (statement->children.for_tree.initializer->kind == ASSIGN) {
				type_node_t* expr_type = type_check_expr(type_env, 
					statement->children.for_tree.condition);
				if (expr_type && expr_type->type == BOOL_T && expr_type->arr_count == 0) {
					free(expr_type);
					// TODO: can updater be a function call? 
				} else {
					// TODO: error message
				}
			} else {
				// TODO: error message 
			}
			break; 
		case WHILE:
			type_node_t* expr_type = type_check_expr(type_env, 
				statement->children.while_tree.condition);
			if (expr_type && expr_type->type == BOOL_T && expr_type->arr_count == 0) {
				free(expr_type);
				success &= type_check(type_env, statement->children.while_tree.block);
			} else {
				// TODO: error message 
			}
		case FUNC:
		case RET:
		case ASSIGN:
		case EXPR:
			type_node_t* expr_type = type_check_expr(type_env, 
				&(statement->children.expr));
			if (expr_type) {
				free(expr_type);
				success &= true; 
			} else success &= false; 
			break; 
		default: return false; 
	}
}

/* 
Purpose: type checks an expression 
Return type: a pointer to a type_node_t structure if the types were correct
	otherwise, a null pointer is returned, symbolizing a type error. 
Misc: callee is expected to free the struct pointer 
*/ 
type_node_t* type_check_expr(symtab_t* type_env, expr_ast_t* expr) {
	
	int arr_count = 0; 
	int type; 

	switch (expr->kind) {
		case BINOP:
			type_node_t* lhs = type_check_expr(type_env, expr->children.binop.lhs);
			type_node_t* rhs = type_check_expr(type_env, expr->children.binop.rhs);
			if (lhs && rhs) {
				switch (expr->children.binop.op) {
					case ADD_NODE:
						if (lhs == rhs && 
							lhs->arr_count == 0 && 
							lhs->type != BOOL_T) {
								type = lhs->type; 
								break; 
							} else {
								// TODO: error message 
							}
					case SUB_NODE:
					case MUL_NODE:
					case DIV_NODE:
						if (lhs == rhs && 
							lhs->.arr_count == 0 && 
							(lhs->.type == INT_T ||
							lhs->.type == DOUBLE_T)) {
								type = lhs->.type;
								break; 
							} else {
								// TODO: error message 
							}
					case EQ_NODE:
					case NEQ_NODE:
						if (lhs == rhs) {
							type = BOOL_T;
							break; 
						} else {
							// TODO: error message 
						}
					case LT_NODE:
					case GT_NODE:
					case LE_NODE:
					case GE_NODE:
						if (lhs == rhs && 
							lhs->arr_count == 0 &&
							(lhs->type == INT_T ||
							lhs->type == DOUBLE_T)) {
								type = BOOL_T; break; 
							} else {
								// TODO: error message 
							}
					case INDEX_NODE: 
						if (lhs->arr_count != 0 && 
							rhs->arr_count == 0 &&
							rhs->type == INT_T
						) {
							type = lhs->type; break; 
						} else {
							// TODO: error message 
						}
					case ASSIGN_NODE: 
						if (expr->children.binop.lhs->kind == ID_L) {
							symbol_t* sym = lookup(type_env, expr->children.binop.lhs->children.str_val);
							if (sym && sym->kind == VAR)  {
								if (sym->type.var_type.var_type == rhs) {
									type = rhs; break; 
								} else {
									// TODO: error message 
								}
							} else {
								// TODO: error message
							}
						} else {
							// TODO: error message
						}

				}
			} else {
				// TODO: some sort of error message
			}
			break; 
		case CALL: 
			symbol_t* sym = lookup(type_env, expr->children.call.func_name);
			if (sym && sym->kind == FUNC) {
				// TODO: type check parameters 
			} 
			// TODO: some sort of error message
		case INT_L: type = INT_T; 
		case DOUBLE_L: type = DOUBLE_T;
		case STR_L: type = STR_T;
		case ID_L: 
			symbol_t* sym = lookup(type_env, expr->children.str_val);
			if (sym && sym->kind == VAR) { 
				type = sym->type.var_type.var_type.type; 
				arr_count =  sym->type.var_type.var_type.arr_count; 
			}
			// TODO: some sort of error message
		case BOOL_L: type = BOOL_T;
		default: return NULL; 
	}

	return type_node(type, arr_count);
}
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

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
	ast->kind = EXPR; 
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
Purpose: the driver function to type check ast 
Return type: a boolean representing whether type checking 
	failed. (similar to os status codes, i.e. 0 for success, 1 for error)
*/ 
bool do_type_check(list_t* program) {
	
	// TODO: error type
	symtab_t* globals = make_global_symtab(program);
	bool failed = false;

	foreach(program, curr) {
		state_ast_t* function = curr->current_ele;

		foreach(function->children.func.params, param) {
			symbol_t* sym = init_var_sym(((id_ast_t*) (param->current_ele))->id_type, 
				((id_ast_t*) (param->current_ele))->name, globals->curr_sid + 1);
			insert(globals, sym);
		}

		failed |= type_check_block(globals, function->children.func.block);
	}

	return failed;

}

/*
Purpose: generates a symbol table containing all top-level members
	Currently, only functions exist as top-level members.
	In the future, it may be expanded to variables and such. 
Return type: a populated symbol table 
*/
symtab_t* make_global_symtab(list_t* program) {
	
	symtab_t* symtab = init_symtab();

	foreach(program, curr) {
		state_ast_t* func = (state_ast_t*) (curr->current_ele); 
		if (func->kind == FUNC) {
			
			if (lookup(symtab, func->children.func.identifier->name)) {
				printf("TYPE ERROR: line %d, pos %d: function %s already exists\n",
					func->line, func->pos, func->children.func.identifier->name); 
			} else {
				// get param types 
				list_t* param_types = init_list();
				foreach(func->children.func.params, param) {
					append(param_types, ((id_ast_t*) ((list_el_t*) param)->current_ele)->id_type);
				}	
				symbol_t* func_sym = init_func_sym(func->children.func.identifier->id_type,
					param_types, func->children.func.identifier->name, 0);
				insert(symtab, func_sym);
			}
		}
	}
	return symtab; 
}

/*
Purpose: type checks a block of statements, is responsible for 
	managing scope. 
Return type: a boolean representing whether type checking 
	failed. (similar to os status codes, i.e. 0 for success, 1 for error)
*/
bool type_check_block(symtab_t* type_env, list_t* block) {
	bool failed = false; 
	enter_scope(type_env);

	foreach(block, curr) {
		failed |= type_check_state(type_env, (state_ast_t*) (curr->current_ele));
	}

	exit_scope(type_env); 
	return failed; 
}

/* 
Purpose: type checks a statement
Return type: a boolean representing whether type checking 
	failed. (similar to os status codes, i.e. 0 for success, 1 for error) 
*/ 
bool type_check_state(symtab_t* type_env, state_ast_t* statement) {
	bool failed = false; 
	switch (statement->kind) {
		case IF: 
			foreach(statement->children.if_tree.if_pairs, curr) {
				type_node_t* expr_type = type_check_expr(type_env, 
					((if_pair_t*) curr->current_ele)->condition);
				if (expr_type->type == ERROR_T) {
					free(expr_type); failed |= true; 
				} else if (expr_type->type == BOOL_T && expr_type->arr_count == 0) {
					free(expr_type);
					failed |= type_check_block(type_env, 
						((if_pair_t*) curr->current_ele)->block);
				} else {
					printf("TYPE ERROR: line %d, pos %d: if condition does not evaluate to a boolean\n",
							statement->line, statement->pos);
					failed |= true; 
				}
			}
			break; 
		case FOR:
			if (statement->children.for_tree.initializer->kind == ASSIGN) {
				type_node_t* expr_type = type_check_expr(type_env, 
					statement->children.for_tree.condition);
				if (expr_type->type == ERROR_T) {
					free(expr_type); failed |= true; 
				} else if (expr_type->type == BOOL_T && expr_type->arr_count == 0) {
					free(expr_type);
					// TODO: only specific binary operations? 
					if (statement->children.for_tree.updater->kind == BINOP) {
						symbol_t* sym = init_var_sym( 
							statement->children.for_tree.initializer->children.assign.identifier->id_type,
							statement->children.for_tree.initializer->children.assign.identifier->name,
							type_env->curr_sid + 1);
						insert(type_env, sym);
						failed |= type_check_block(type_env, statement->children.for_tree.block);
						break; 
					} else {
						printf("TYPE ERROR: line %d, pos %d: for updater is not a binary operation\n",
							statement->line, statement->pos); failed |= true; 
					}
				} else {
					if (!expr_type) {
						printf("TYPE ERROR: line %d, pos %d: for condition is not an expression\n",
							statement->line, statement->pos);
					} else {
						printf("TYPE ERROR: line %d, pos %d: for condition does not evaluate to a boolean\n",
							statement->line, statement->pos);
					}
					failed |= true; 
				}
			} else {
				printf("TYPE ERROR: line %d, pos %d: for loop should have variable declaration first\n",
					statement->line, statement->pos); failed |= true; 
			}
			break; 
		case WHILE: {
			type_node_t* expr_type = type_check_expr(type_env, 
				statement->children.while_tree.condition);
			if (expr_type->type == ERROR_T) {
				free(expr_type); failed |= true; 
			} else if (expr_type->type == BOOL_T && expr_type->arr_count == 0) {
				free(expr_type);
				failed |= type_check_block(type_env, statement->children.while_tree.block);
			} else {
				if (!expr_type) {
					printf("TYPE ERROR: line %d, pos %d: while condition is not an expression\n",
						statement->line, statement->pos);
				} else {
					printf("TYPE ERROR: line %d, pos %d: while condition does not evaluate to a boolean\n",
						statement->line, statement->pos);
				}
				failed |= true; 
			}
			break; 
		}
		case FUNC:
			// add parameters to new scope 
			printf("TYPE ERROR: line %d, pos %d: cannot declare function inside block\n",
				statement->line, statement->pos);
			failed |= true; 
			break; 
		case RET:
			// TODO: check return types 
			/* will be type checked in previous traversal */ break; 
		case ASSIGN:
			if (lookup(type_env, statement->children.assign.identifier->name)) {
				printf("TYPE ERROR: line %d, pos %d: variable %s redeclared in scope\n",
					statement->line, statement->pos, statement->children.assign.identifier->name);
				failed |= true; 
				break; 
			} else {
				symbol_t* sym = init_var_sym(statement->children.assign.identifier->id_type,
					statement->children.assign.identifier->name, type_env->curr_sid);
				insert(type_env, sym);
				break; 
			}
		case EXPR: {
			type_node_t* expr_type = type_check_expr(type_env, 
				&(statement->children.expr));
			if (expr_type->type == ERROR_T) {
				free(expr_type); failed |= true; 
			} else if (expr_type) {
				free(expr_type);
				failed |= false; 
			}
			break; 
		}

		default: return false; 
	}

	return failed; 
}

/* 
Purpose: type checks an expression 
Return type: a pointer to a type_node_t structure if the types were correct
	otherwise, a null pointer is returned, symbolizing a type error. 
Misc: callee is expected to free the struct pointer 
*/ 
type_node_t* type_check_expr(symtab_t* type_env, expr_ast_t* expr) {
	
	int arr_count = 0; 
	int type = 0; 

	switch (expr->kind) {
		case BINOP: {
			type_node_t* lhs = type_check_expr(type_env, expr->children.binop.lhs);
			type_node_t* rhs = type_check_expr(type_env, expr->children.binop.rhs);

			if (lhs->type == ERROR_T || rhs->type == ERROR_T) {
				free(lhs); free(rhs);
				type = ERROR_T; break; 
			}

			switch (expr->children.binop.op) {
				case ADD_NODE:
					if (type_cmp(lhs, rhs) && 
						lhs->arr_count == 0 && 
						lhs->type != BOOL_T) type = lhs->type; 
					else {
						if (!type_cmp(lhs, rhs)) {
							printf("TYPE ERROR: line %d, pos %d: can only add identical types\n",
								expr->line, expr->pos);
						} else if (lhs->arr_count) {
							printf("TYPE ERROR: line %d, pos %d: cannot add arrays\n",
								expr->line, expr->pos);
						} else {
							printf("TYPE ERROR: line %d, pos %d: cannot add booleans\n",
								expr->line, expr->pos);
						}
					}
					break; 
				case SUB_NODE:
				case MUL_NODE:
				case DIV_NODE:
					if (type_cmp(lhs, rhs) && 
						lhs->arr_count == 0 && 
						(lhs->type == INT_T ||
						lhs->type == DOUBLE_T)) type = lhs->type;
						else {
							if (!type_cmp(lhs, rhs)) {
								printf("TYPE ERROR: line %d, pos %d: can only use identical types\n",
									expr->line, expr->pos);
							} else if (lhs->arr_count) {
								printf("TYPE ERROR: line %d, pos %d: cannot be an array\n",
									expr->line, expr->pos);
							} else {
								printf("TYPE ERROR: line %d, pos %d: can only be integer or double\n",
									expr->line, expr->pos);
							}

						}
						break; 
				case EQ_NODE:
				case NEQ_NODE:
					if (type_cmp(lhs, rhs)) type = BOOL_T; 
					else {
						printf("TYPE ERROR: line %d, pos %d: can only compare identical types\n",
									expr->line, expr->pos);
					}
					break; 
				case LT_NODE:
				case GT_NODE:
				case LE_NODE:
				case GE_NODE:
					if (type_cmp(lhs, rhs) && 
						lhs->arr_count == 0 &&
						(lhs->type == INT_T ||
						lhs->type == DOUBLE_T)) type = BOOL_T; 
						else {
							if (!type_cmp(lhs, rhs)) {
								printf("TYPE ERROR: line %d, pos %d: can only compare identical types\n",
									expr->line, expr->pos);
							} else if (!(lhs->type == INT_T || lhs->type == DOUBLE_T)) {
								printf("TYPE ERROR: line %d, pos %d: can only compare integers or doubles\n",
									expr->line, expr->pos);
							}

						}
						break; 
				case INDEX_NODE: 
					if (lhs->arr_count != 0 && 
						rhs->arr_count == 0 &&
						rhs->type == INT_T
					) type = lhs->type;  
					else {
						if (lhs->arr_count == 0) {
							printf("TYPE ERROR: line %d, pos %d: %s is not an array\n",
								expr->line, expr->pos, expr->children.binop.lhs->children.str_val);
						} else if (rhs->arr_count != 0) {
							printf("TYPE ERROR: line %d, pos %d: index cannot be an array\n",
								expr->line, expr->pos);
						} else {
							printf("TYPE ERROR: line %d, pos %d: index should be an integer\n",
								expr->line, expr->pos);
						}
					}
					break; 
				case ASSIGN_NODE: 
					if (expr->children.binop.lhs->kind == ID_L) {
						symbol_t* sym = lookup(type_env, expr->children.binop.lhs->children.str_val);
						if (sym && sym->kind == VAR_SYM)  {
							if (type_cmp(sym->type.var_type, rhs)) type = rhs->type;
							else {
								char* var_str = type_str(sym->type.var_type);
								char* actual_str = type_str(rhs);
								printf("TYPE ERROR: line %d, pos %d: variable %s is of type %s but received type %s\n",
									expr->line, expr->pos, expr->children.binop.lhs->children.str_val,
									var_str, actual_str);
								free(var_str); free(actual_str); 
							}
						} else {
							if (sym) {
								printf("TYPE ERROR: line %d, pos %d: %s is not a variable\n",
									expr->line, expr->pos, expr->children.binop.lhs->children.str_val);
							} else {
								printf("TYPE ERROR: line %d, pos %d: %s does not exist in scope\n",
									expr->line, expr->pos, expr->children.binop.lhs->children.str_val);			
							}
						}
					} else {
						printf("TYPE ERROR: line %d, pos %d: can only peform assignment on identifiers\n",
									expr->line, expr->pos);	
					}
					break; 
			}
			break; 
		}
		case CALL: {
			bool erroneous = false; 
			symbol_t* sym = lookup(type_env, expr->children.call.func_name);
			if (sym && sym->kind == FUNC_SYM) {
				list_el_t* expected_types = sym->type.func_signature.param_types->head; 
				list_t* param_types = init_list();

				foreach(expr->children.call.params, param) { 
					type_node_t* param_type = type_check_expr(type_env, param->current_ele);
					if (param_type->type == ERROR_T) {
						free(param_type); 
						type = ERROR_T; 
						goto END; 
					} else append(param_types, param_type); 
				}

				foreach(param_types, curr) {
					if (!expected_types) {
						printf("TYPE ERROR: line %d, pos %d: function call has too many parameters\n",
							expr->line, expr->pos);
						break; 
					}

					type_node_t* actual = curr->current_ele;
					type_node_t* formal = expected_types->current_ele;

					if (!type_cmp(actual, formal)) {
						erroneous = 1;
						char* actual_str = type_str(actual);
						char* formal_str = type_str(formal);
						printf("TYPE ERROR: line %d, pos %d: function parameter is expected to be type %s but received type %s\n",
							expr->line, expr->pos, formal_str, actual_str);
						free(actual_str); free(formal_str);
					}

					expected_types = expected_types->next;
				}

				if (expected_types) {
					printf("TYPE ERROR: line %d, pos %d: function call has too few parameters\n",
						expr->line, expr->pos);
				}

				// free_list(param_types);

				if (!erroneous) {
					type = sym->type.func_signature.ret_type->type;
					arr_count = sym->type.func_signature.ret_type->arr_count;
				} 
			} else {
				if (sym) {
					printf("TYPE ERROR: line %d, pos %d: %s is not a function\n",
						expr->line, expr->pos, expr->children.call.func_name);
				} else {
					printf("TYPE ERROR: line %d, pos %d: %s does not exist\n",
						expr->line, expr->pos, expr->children.call.func_name);
				}
			}
			break; 
		}
		case INT_L: type = INT_T; break; 
		case DOUBLE_L: type = DOUBLE_T; break; 
		case STR_L: type = STR_T; break; 
		case ID_L: {
			symbol_t* sym = lookup(type_env, expr->children.str_val);
			if (sym && sym->kind == VAR_SYM) { 
				type = sym->type.var_type->type;
				arr_count = sym->type.var_type->arr_count;
			} else {
				if (sym) {
					printf("TYPE ERROR: line %d, pos %d: %s is not a variable\n",
						expr->line, expr->pos, expr->children.str_val);
				} else {
					printf("TYPE ERROR: line %d, pos %d: %s does not exist\n",
						expr->line, expr->pos, expr->children.str_val);
				}
			}
			break;
		}
		case BOOL_L: type = BOOL_T; break; 
		default: type = ERROR_T; 
	}

	if (!type) type = ERROR_T; 

	END: 
	return type_node(type, arr_count);
}
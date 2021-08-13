#include <stdlib.h>

#include "includes/ast.h"
#include "includes/list.h"
#include "includes/utils.h"

// constructors for expressions 
expr_ast_t* int_node(int val, int line, int pos) {
	expr_ast_t* node = malloc(sizeof(expr_ast_t));
	node->kind = INT_L;
	node->line = line;
	node->pos = pos; 
	node->children.int_val = val;
	return node; 
}

expr_ast_t* str_node(char* start, size_t length, int line, int pos) {
	expr_ast_t* node = malloc(sizeof(expr_ast_t)); 
	node->kind = STR_L;
	node->line = line;
	node->pos = pos;
	node->children.str_val = substring(start, length); 
	return node; 
}

expr_ast_t* double_node(double val, int line, int pos) {
	expr_ast_t* node = malloc(sizeof(expr_ast_t));
	node->kind = DOUBLE_L;
	node->line = line;
	node->pos = pos; 
	node->children.double_val = val;
	return node; 
}

expr_ast_t* id_node(char* start, size_t length, int line, int pos) {
	expr_ast_t* node = str_node(start, length, line, pos); 
	node->kind = ID_L;
	return node; 
}

expr_ast_t* bool_node(int val, int line, int pos) {
	expr_ast_t* node = int_node(val, line, pos);
	node->kind = BOOL_L;
	return node; 
}

expr_ast_t* call_ast(char* func, list_t* params, int line, int pos) {
	expr_ast_t* ast = malloc(sizeof(expr_ast_t));
	ast->line = line;
	ast->pos = pos;
	ast->kind = CALL;
	ast->children.call = (struct call_ast) {
		.func_name = func, .params = params 
	};
	return ast; 
}

expr_ast_t* binop_ast(int op, expr_ast_t* lhs, expr_ast_t* rhs, int line, int pos) {
	expr_ast_t* ast = malloc(sizeof(expr_ast_t)); 
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
	state_ast_t* ast = malloc(sizeof(state_ast_t));
	ast->kind = IF;
	ast->line = line; 
	ast->pos = pos;
	ast->children.if_tree.if_pairs = if_pairs; 
	return ast; 
}

state_ast_t* for_ast(state_ast_t* initializer, expr_ast_t* condition, state_ast_t* updater, list_t* block, int line, int pos) {
	state_ast_t* ast = malloc(sizeof(state_ast_t)); 
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
	state_ast_t* ast = malloc(sizeof(state_ast_t)); 
	ast->children.while_tree = (struct while_ast) {
		.condition = condition,
		.block = block
	};
	ast->kind = WHILE; 
	ast->line = line;
	ast->pos = pos; 
	return ast; 
}

state_ast_t* func_ast(struct id_ast identifier, list_t* params, list_t* block, int line, int pos) {
	state_ast_t* ast = malloc(sizeof(state_ast_t));
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
	state_ast_t* ast = malloc(sizeof(state_ast_t)); 
	ast->children.ret.expression = expr;
	ast->line = line;
	ast->kind = RET; 
	ast->pos = pos;
	return ast; 
}

state_ast_t* assign_ast(struct id_ast* id, expr_ast_t* val, int line, int pos) {
	state_ast_t* ast = malloc(sizeof(state_ast_t));
	ast->children.assign = (struct assign_ast) {
		.identifier = id,
		.value = val
	};
	ast->line = line;
	ast->pos = pos; 
	ast->kind = ASSIGN; 
	return ast; 
}

// misc ast nodes and trees
if_pair_t* if_pair(expr_ast_t* condition, list_t* block) {
	if_pair_t* pair = malloc(sizeof(if_pair_t)); 
	pair->condition = condition;
	pair->block = block; 
	return pair; 
}

type_node_t* type_node(int type, int arr_count) {
	type_node_t* node = malloc(sizeof(type_node_t));
	node->type = type;
	node->arr_count = arr_count; 
	return node; 
}

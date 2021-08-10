#include <stdlib.h>

#include "includes/ast.h"
#include "includes/list.h"
#include "utils.c"

// constructors for expressions 
expr_ast_t* int_node(int val, int line, int pos) {
	expr_ast_t* node = malloc(sizeof(expr_ast_t));
	node->kind = INT;
	node->line = line;
	node->pos = pos; 
	node->children.int_val = val;
	return node; 
}

expr_ast_t* str_node(char* start, size_t length, int line, int pos) {
	expr_ast_t* node = malloc(sizeof(expr_ast_t)); 
	node->kind = STR;
	node->line = line;
	node->pos = pos;
	node->children.str_val = substring(start, length); 
	return node; 
}

expr_ast_t* double_node(double val, int line, int pos) {
	expr_ast_t* node = malloc(sizeof(expr_ast_t));
	node->kind = DOUBLE;
	node->line = line;
	node->pos = pos; 
	node->children.double_val = val;
	return node; 
}

expr_ast_t* id_node(char* start, size_t length, int line, int pos) {
	expr_ast_t* node = str_node(start, length, line, pos); 
	node->kind = ID;
	return node; 
}

expr_ast_t* bool_node(int val, int line, int pos) {
	expr_ast_t* node = int_node(val, line, pos);
	node->kind = BOOL;
	return node; 
}

expr_ast_t* call_ast(char* func, list_t* params, int line, int pos) {
	expr_ast_t* ast = malloc(sizeof(expr_ast_t));
	ast->line = line;
	ast->pos = pos;
	ast->kind = CALL;
	ast->children.call.func_name = func;
	ast->children.call.params = params;
	return ast; 
}

expr_ast_t* binop_ast(int op, expr_ast_t* lhs, expr_ast_t* rhs, int line, int pos) {
	expr_ast_t* ast = malloc(sizeof(expr_ast_t)); 
	ast->line = line;
	ast->pos = pos;
	ast->children.binop.rhs = rhs;
	ast->children.binop.lhs = lhs;
	ast->children.binop.op = op;
	return ast; 
}

// constructors for statements


#include <stdlib.h>

#include "includes/ast.h"
#include "utils.c"

expr_ast_s* int_node(int val, int line, int pos) {
	expr_ast_s* node = malloc(sizeof(expr_ast_s));
	node->kind = INT;
	node->line = line;
	node->pos = pos; 
	node->children.int_val = val;
	return node; 
}

expr_ast_s* str_node(char* start, size_t length, int line, int pos) {
	expr_ast_s* node = malloc(sizeof(expr_ast_s)); 
	node->kind = STR;
	node->line = line;
	node->pos = pos;
	node->children.str_val = substring(start, length); 
	return node; 
}

expr_ast_s* double_node(double val, int line, int pos) {
	expr_ast_s* node = malloc(sizeof(expr_ast_s));
	node->kind = DOUBLE;
	node->line = line;
	node->pos = pos; 
	node->children.double_val = val;
	return node; 
}

expr_ast_s* id_node(char* start, size_t length, int line, int pos) {
	expr_ast_s* node = str_node(start, length, line, pos); 
	node->kind = ID;
	return node; 
}

expr_ast_s* bool_node(int val, int line, int pos) {
	expr_ast_s* node = int_node(val, line, pos);
	node->kind = BOOL;
	return node; 
}

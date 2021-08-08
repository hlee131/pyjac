#include <stdlib.h>

#include "includes/ast.h"

AST* init_ast() {
	AST* ast = malloc(sizeof(AST));
	ast->kind = NODE_PROGRAM;
	return ast; 
}

void attach_to_ast(AST* parent, AST* child) {
	parent->value.children 
}

#ifndef AST_H
#define AST_H
#include <stdbool.h>
#include "list.h" 
#include "symtab.h"

typedef struct expr_ast_s expr_ast_t; 
typedef struct state_ast_s state_ast_t; 
typedef struct id_ast_s id_ast_t; 
typedef struct type_node_s type_node_t; 


// ast for binary operations, i.e. +, -, ...
struct binop_ast {
	enum {
		ADD_NODE = 1, SUB_NODE, MUL_NODE, DIV_NODE,
		EQ_NODE, NEQ_NODE, LT_NODE, GT_NODE, LE_NODE,
		GE_NODE, INDEX_NODE, ASSIGN_NODE 
	} op; 

	expr_ast_t* lhs;
	expr_ast_t* rhs; 
};

struct decl_ast {
	id_ast_t* identifier; 
	expr_ast_t* value; 
};

// ast for if statements
// block contains the code inside the statement
struct if_ast {
	list_t* if_pairs; 
};

// pairs a condition to a block of code
// represents if, elif, and else 
typedef struct if_pair_s {
	expr_ast_t* condition;
	list_t* block; 
} if_pair_t; 

// ast for for loops 
struct for_ast {
	state_ast_t* initializer;
	expr_ast_t* condition;
	expr_ast_t* updater;
	list_t* block; 
};

// ast for while loops
struct while_ast {
	expr_ast_t* condition;
	list_t* block; 
};

// structure for types in the ast 
typedef struct type_node_s {
	enum { INT_T = 1, STR_T, BOOL_T, DOUBLE_T } type;
	// keeps track of arrays if any 
	// e.g. Arr:Int: array of ints would have type = int and arr_count = 1
	int arr_count; 
} type_node_t;

// ast for identifiers 
//
// is used to represent an identifier in an ast 
typedef struct id_ast_s {
	char* name;
	type_node_t* id_type; 
} id_ast_t;

// ast for function declarations 
struct func_ast {
	id_ast_t* identifier; 
	// params will be list of id_ast 
	list_t* params; 
	list_t* block; 
};


// used when a function is called
struct call_ast {
	char* func_name;
	list_t* params; 
}; 

// used for return statements
struct ret_ast {
	expr_ast_t* expression; 
}; 

typedef struct expr_ast_s {
	enum { BINOP = 1, CALL, INT_L, DOUBLE_L, STR_L, ID_L, BOOL_L } kind;

	int line; 
	int pos;

	union {
		struct binop_ast binop; 
		struct call_ast call; 

		int int_val;
		double double_val;
		char* str_val; 
	} children; 

} expr_ast_t; 

typedef struct state_ast_s {
	enum { IF = 1, FOR, WHILE, FUNC, RET, ASSIGN, EXPR } kind; 
	
	int line;
	int pos;

	union {
		struct if_ast if_tree;
		struct for_ast for_tree;
		struct while_ast while_tree;
		struct func_ast func;
		struct ret_ast ret; 
		struct decl_ast assign; 
		struct expr_ast_s expr; 
	} children; 

} state_ast_t; 

// constructors
expr_ast_t* int_node(int val, int line, int pos);
expr_ast_t* str_node(char* value, int line, int pos);
expr_ast_t* double_node(double val, int line, int pos); 
expr_ast_t* id_node(char* value, int line, int pos);
expr_ast_t* bool_node(int val, int line, int pos);
expr_ast_t* call_ast(char* func, list_t* params, int line, int pos); 
expr_ast_t* binop_ast(int op, expr_ast_t* lhs, expr_ast_t* rhs, int line, int pos);
state_ast_t* if_ast(list_t* if_pairs, int line, int pos);
state_ast_t* for_ast(state_ast_t* initializer, expr_ast_t* condition, expr_ast_t* updated, list_t* block, int line, int pos);
state_ast_t* while_ast(expr_ast_t* condition, list_t* block, int line, int pos);
state_ast_t* func_ast(id_ast_t* identifier, list_t* params, list_t* block, int line, int pos); 
state_ast_t* ret_ast(expr_ast_t* expr, int line, int pos);
state_ast_t* decl_ast(id_ast_t* id, expr_ast_t* val, int line, int pos);
state_ast_t* expr_ast(expr_ast_t* expr, int line, int pos);
if_pair_t* if_pair(expr_ast_t* condition, list_t* block); 
type_node_t* type_node(int type, int arr_count);
id_ast_t* id_ast(char* name, type_node_t* id_type); 

// semantic analysis methods
bool do_type_check(list_t* program); 
symtab_t* make_global_symtab(list_t* program);
bool type_check_block(symtab_t* type_env, list_t* block);
bool type_check_state(symtab_t* type_env, state_ast_t* statement);
type_node_t* type_check_expr(symtab_t* type_env, expr_ast_t* expr);

#endif

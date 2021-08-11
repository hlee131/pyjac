#ifndef AST_H
#define AST_H
#include "list.h" 

typedef struct expr_ast_s expr_ast_t; 
typedef struct state_ast_s state_ast_t; 
struct id_ast; 
typedef struct type_node_s type_node_t; 


// ast for binary operations, i.e. +, -, ...
struct binop_ast {
	enum {
		ADD_NODE, SUB_NODE, MUL_NODE, DIV_NODE,
		EQ_NODE, NEQ_NODE, LT_NODE, GT_NODE, LE_NODE,
		GE_NODE, INDEX_NODE 
	} op; 

	expr_ast_t* lhs;
	expr_ast_t* rhs; 
};

struct assign_ast {
	struct id_ast* identifier; 
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
	state_ast_t* updater;
	list_t* block; 
};

// ast for while loops
struct while_ast {
	expr_ast_t* condition;
	list_t* block; 
};

// structure for types in the ast 
typedef struct type_node_s {
	enum { INT_T, STR_T, BOOL_T, DOUBLE_T } type;
	// keeps track of arrays if any 
	// e.g. Arr:Int: array of ints would have type = int and arr_count = 1
	int arr_count; 
} type_node_t;

// ast for identifiers 
//
// is used to represent an identifier in an ast 
struct id_ast {
	char* name;
	type_node_t* id_type; 
};

// ast for function declarations 
struct func_ast {
	struct id_ast identifier; 
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
	enum { BINOP, CALL, INT_L, DOUBLE_L, STR_L, ID_L, BOOL_L } kind;

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
	enum { IF, FOR, WHILE, FUNC, RET, ASSIGN } kind; 
	
	int line;
	int pos;

	union {
		struct if_ast if_tree;
		struct for_ast for_tree;
		struct while_ast while_tree;
		struct func_ast func;
		struct ret_ast ret; 
		struct assign_ast assign; 
	} children; 

} state_ast_t; 

#endif

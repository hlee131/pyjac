#ifndef AST_H
#define AST_H

// ast for binary operations, i.e. +, -, ...
struct binop_ast {
	enum {
		ADD_NODE, SUB_NODE, MUL_NODE, DIV_NODE,
		EQ_NODE, NEQ_NODE, LT_NODE, GT_NODE, LE_NODE,
		GE_NODE, ASSIGN_NODE, INDEX_NODE 
	} op; 

	expr_ast_t* lhs;
	expr_ast_t* rhs; 
}

// ast for if statements
// TODO: add ast for elif and else 
// block contains the code inside the statement
struct if_ast {
	expr_ast_t* condition;
	list_t* block; 
}

// ast for for loops 
struct for_ast {
	state_ast_t* initializer;
	expr_ast_t* condition;
	state_ast_t* updater;
	list_t* block; 
}

// ast for while loops
struct while_ast {
	expr_ast_t* condition;
	list_t* block; 
}

// ast for function declarations 
struct func_ast {
	struct id_ast identifier; 
	list_t* params; 
	list_t* block; 
}

// ast for identifiers 
//
// opposed to the ID_NODE in AST_node_type, id_ast 
// is used when a type is associated with an identifer 
struct id_ast {
	char* name;
	type_node id_type; 
}

// used when a function is called
struct call_ast {
	char* func_name;
	list_t* params; 
}; 

// used for return statements
struct ret_ast {
	AST* expression; 
}; 

typedef struct expr_ast_s {
	enum { BINOP, CALL, INT, DOUBLE, STR, ID } kind;

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
	enum { IF, FOR, WHILE, FUNC, RET } kind; 
	
	int line;
	int pos;

	union {
		struct if_ast if;
		struct for_ast for;
		struct while_ast while;
		struct func_ast func;
		struct ret_ast ret; 
	} children; 

} state_ast_t; 

// structure for types in the ast 
typedef struct tn {
	enum { INT, STR, BOOL, DOUBLE } type;
	// keeps track of arrays if any 
	// e.g. Arr:Int: array of ints would have type = int and arr_count = 1
	int arr_count; 
} type_node; 
#endif

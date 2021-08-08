#ifndef AST_H
#define AST_H

typedef enum t {
	// PROGRAM is the head of the tree
	PROGRAM_NODE, 
	
	// constants
	NODE_INT,
	NODE_STR,
	NODE_DOUBLE,
	NODE_BOOL, 

	// operations
	ADD_NODE,
	SUB_NODE,
	MUL_NODE,
	DIV_NODE,
	EQ_NODE,
	NEQ_NODE,
	LT_NODE,
	GT_NODE,
	LE_NODE,
	GE_NODE,
	ASSIGN_NODE,
	IF_NODE,
	ELSE_NODE,
	WHILE_NODE,
	FOR_NODE,
	FUNC_NODE,
	RET_NODE
	IF_NODE,
	ELIF_NODE,
	ELSE_NODE,
	INDEX_NODE,
	CALL_NODE,

	// used for reassignment, parameters, etc. 
	ID_NODE, 
	
} AST_node_type; 

// ast for binary operations, i.e. +, -, ...
struct binop_ast {
	AST* lhs;
	AST* rhs; 
}

// ast for if statements
// block contains the code inside the statement
struct if_ast {
	AST* condition;
	AST_linked_list* block; 
}

// ast for for loops 
struct for_ast {
	AST* initializer;
	AST* condition;
	AST* updater;
	AST_linked_list* block; 
}

// ast for while loops
struct while_ast {
	AST* condition;
	AST_linked_list* block; 
}

// ast for function declarations 
struct func_ast {
	type_node ret_type;
	
	struct { 
		id_ast* current;
		id_ast* next; 
	} params; 

	AST_linked_list* block; 
}

// ast for identifiers 
//
// opposed to the ID_NODE in AST_node_type, id_ast 
// is used during the declaration of variables 
struct id_ast {
	char* name;
	type_node id_type; 
}

// used when a function is called
struct call_ast {
	AST* function;
	struct {
		AST* current;
		AST* next;
	} params; 
}; 

// used for return statements
struct ret_ast {
	AST* expression; 
}; 

// main ast structure
typedef struct a {
	AST_node_type kind;

	// src location for error messages
	int line;
	int pos; 

	union {
		// constants info 
		int int_value;
		double double_val;
		// also used to hold identifer names
		char* str_val;

		// subtree info 
		struct binop_ast binop_tree;
		struct if_ast if_tree;
		struct for_ast for_tree;
		struct while_ast while_tree;
		struct func_ast func_tree; 
		struct call_ast call_tree;
		struct ret_ast ret_tree; 
	} value; 
} AST; 

// structure for types in the ast 
typedef struct tn {
	enum { INT, STR, BOOL, DOUBLE } type;
	// keeps track of arrays if any 
	// e.g. Arr:Int: array of ints would have type = int and arr_count = 1
	int arr_count; 
} type_node; 

// linked list to keep track of blocks of code 
typedef struct ll {
	AST* current;
	AST* next; 
} AST_linked_list; 
#endif

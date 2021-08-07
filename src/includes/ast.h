#ifndef AST_H
#define AST_H

typedef enum t {
	// PROGRAM is the head of the tree
	PROGRAM_NODE, 
	
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
		
	

	
} AST_node_type; 


typedef struct a {
	AST_node_type kind;

	union {
		int int_value;
		double double_val;
		char* str_val;
		AST_linked_list* block;

	} value; 
} AST; 

typedef struct ll {
	AST* current;
	AST* next; 
} AST_linked_list; 

#endif

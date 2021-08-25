#ifndef SYMTAB_H
#define SYMTAB_H
#include "list.h"
#include "ast.h"

#define BUCKET_COUNT 13

typedef struct symtab_s {
    list_t* stacks[BUCKET_COUNT]; 
    int curr_sid; 
} symtab_t; 

typedef struct symbol_s {
    enum { VAR, FUNC } kind;
    
    // points to a func_type_t or a var_type_t
    void* type_ptr; 

    char* identifier; 
    int scope_id; 
} symbol_t; 

typedef struct func_type_s {
    type_node_t ret_type;
    list_t* param_types; 
} func_type_t;

typedef struct var_type_s {
    type_node_t var_type; 
} var_type_t;

symtab_t* init_symtab(); 
symbol_t* lookup(symtab_t* table, char* key);
void insert(symtab_t* table, symbol_t* symbol); 
symbol_t* init_symbol(int kind, void* type_ptr, char* identifier, int sid);
int get_index(char* key);
void exit_scope(symtab_t* table);
static inline void enter_scope(symtab_t* table);
#endif

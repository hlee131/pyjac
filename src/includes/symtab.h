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
    type_node_t type;
    char* identifier; 
    int scope_id; 
} symbol_t; 

symtab_t* init_symtab(); 
symbol_t* lookup(symtab_t* table, char* key);
void insert(symtab_t* table, symbol_t* symbol); 
symbol_t* init_symbol(int kind, type_node_t type, char* identifier, int sid);
int get_index(char* key);
void exit_scope(symtab_t* table);
static inline void enter_scope(symtab_t* table);
#endif

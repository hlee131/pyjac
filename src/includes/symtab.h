#ifndef SYMTAB_H
#define SYMTAB_H
#include "list.h"
#include "ast.h"

#define BUCKET_COUNT 13
#define enter_scope(symtab) (table->curr_side++)

typedef struct symtab_s {
    list_t* stacks[BUCKET_COUNT]; 
    int curr_sid; 
} symtab_t; 

typedef struct symbol_s {
    enum { VAR_SYM, FUNC_SYM } kind;
    
    // points to a func_type_t or a var_type_t
    union {
        struct {
            struct type_node_s* ret_type;
            list_t* param_types; 
        } func_signature; 
        struct type_node_s* var_type; 
    } type; 

    char* identifier; 
    int scope_id; 
} symbol_t; 


symtab_t* init_symtab(); 
symbol_t* lookup(symtab_t* table, char* key);
void insert(symtab_t* table, symbol_t* symbol); 
symbol_t* init_var_sym(struct type_node_s* type, char* name, int sid);
symbol_t* init_func_sym(struct type_node_s* ret_type, list_t* param_types, char* name, int sid);
int get_index(char* key);
void exit_scope(symtab_t* table);
#endif

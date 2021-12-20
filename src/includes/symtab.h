#ifndef SYMTAB_H
#define SYMTAB_H
#include <llvm-c/Core.h>

#include "list.h"
#include "ast.h"
#include "parser.h"

#define BUCKET_COUNT 13
#define enter_scope(symtab) (symtab->curr_sid++)

typedef struct symtab_s {
    list_t* stacks[BUCKET_COUNT]; 
    int curr_sid; 
    type_node_t* expected_type; 
    LLVMValueRef curr_func; 
} symtab_t; 

typedef struct symbol_s {
    enum { VAR_SYM, FUNC_SYM } kind;
    
    // points to a func_type_t or a var_type_t
    union {
        struct {
            struct type_node_s* ret_type;
            type_list_t param_types; 
        } func_signature; 
        struct type_node_s* var_type; 
        LLVMValueRef val_ref; 
    } type; 

    char* identifier; 
    int scope_id; 
} symbol_t; 


symtab_t* init_symtab(); 
symbol_t* lookup(symtab_t* table, char* key);
void insert(symtab_t* table, symbol_t* symbol); 
symbol_t* init_var_sym(struct type_node_s* type, char* name, int sid);
symbol_t* init_func_sym(struct type_node_s* ret_type, type_list_t param_types, char* name, int sid);
int get_index(char* key);
void exit_scope(symtab_t* table);
void insert_LLVM_ref(LLVMValueRef ref, int type, char* identifier, symtab_t* table);
#endif

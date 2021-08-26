#include <string.h>
#include "includes/symtab.h"
#include "includes/utils.h"
#include "includes/list.h"

symtab_t* init_symtab() {
    symtab_t* table = checked_malloc(sizeof(symtab_t));
    table->curr_sid = 0; 
    for (int i = 0; i < BUCKET_COUNT; i++) {
        table->stacks[i] = init_list();
    }
}

symbol_t* init_var_sym(type_node_t* type, char* name, int sid) {
    symbol_t* sym = checked_malloc(sizeof(symbol_t));
    sym->kind = VAR_SYM;
    sym->identifier = name;
    sym->scope_id = sid;
    sym->type.var_type = type;
    return sym; 
}

symbol_t* init_func_sym(type_node_t* ret_type, list_t* param_types, char* name, int sid) {
    symbol_t* sym = checked_malloc(sizeof(symbol_t));
    sym->kind = FUNC_SYM;
    sym->identifier = name;
    sym->scope_id = sid; 
    sym->type.func_signature.param_types = param_types;
    sym->type.func_signature.ret_type = ret_type;
    return sym; 
}

int get_index(char* key) {
    int sum = 0;
    while (*key != 0) sum += (int) key++; 
    return sum % BUCKET_COUNT; 
}

symbol_t* lookup(symtab_t* table, char* key) {
    int index = get_index(key);
    
    foreach(table->stacks[index]) {
        if (strcmp(((symbol_t*) curr->current_ele)->identifier, key) == 0) {
            return curr->current_ele; 
        }
    }

    return NULL; 
}

void insert(symtab_t* table, symbol_t* symbol) {
    int index = get_index(symbol->identifier);
    append_head(table->stacks[index], symbol); 
}

void exit_scope(symtab_t* table) {
    for (int i = 0; i < BUCKET_COUNT; i++) {
        foreach(table->stacks[i]) {
            if (((symbol_t *) curr->current_ele)->scope_id == table->curr_sid) {
                free(curr->current_ele);
                free(curr); 
                table->stacks[i]->head = curr->next;
            } else break; 
        }
    }

    table->curr_sid--; 
}

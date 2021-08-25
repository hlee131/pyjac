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

symbol_t* init_symbol(int kind, void* type_ptr, char* identifier, int sid) {
    symbol_t* symbol = checked_malloc(sizeof(symbol_t));
    
    if (kind == VAR) symbol->type_ptr = (var_type_t*) type_ptr; 
    else symbol->type_ptr = (func_type_t*) type_ptr; 

    symbol->identifier = identifier;
    symbol->scope_id = sid; 
    return symbol; 
}

int get_index(char* key) {
    int sum = 0;
    while (*key != 0) sum += (int) key++; 
    return sum % BUCKET_COUNT; 
}

symbol_t* lookup(symtab_t* table, char* key) {
    int index = get_index(key);
    
    list_el_t* next = table->stacks[index]->head;

	while (next) {
		list_el_t* curr = next; 
		next = next->next; 
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

static inline void enter_scope(symtab_t* table) {
    table->curr_sid++; 
}

void exit_scope(symtab_t* table) {
    for (int i = 0; i < BUCKET_COUNT; i++) {
        list_el_t* next = table->stacks[i]->head;

        while (next) {
            list_el_t* curr = next; 
            next = next->next; 
            if (((symbol_t *) curr->current_ele)->scope_id == table->curr_sid) {
                free(curr->current_ele);
                free(curr); 
                table->stacks[i]->head = next;
            } else break; 
        } 
    }

    table->curr_sid--; 
}

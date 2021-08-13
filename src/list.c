#include <stdlib.h> 

#include "includes/list.h"

list_t* init_list() {
	list_t* list = malloc(sizeof(list_t)); 
	list->head = list->tail = NULL;
	list->length = 0;
	return list; 
}

void append(list_t* list, void* value) {
	list_el_t* new_node = malloc(sizeof(list_el_t));
	new_node->current_ele = value; 
	new_node->next = NULL; 

	if (list->length == 0) {
		list->head = list->tail = new_node; 
	} else { 
		list->tail->next = new_node; 
		list->tail = new_node; 
	}

	list->length++; 
}

// TODO: recursively free the tree
void free_list(list_t* list) {
	list_el_t* next = list->head;

	while (next) {
		list_el_t* curr = next; 
		next = next->next; 
		free(curr->current_ele);
		free(curr); 
	} 

	free(list); 
}

#include <stdlib.h> 

#include "includes/list.h"
#include "includes/utils.h"

list_t* init_list() {
	list_t* list = checked_malloc(sizeof(list_t)); 
	list->head = list->tail = NULL;
	list->length = 0;
	return list; 
}

void append(list_t* list, void* value) {
	list_el_t* new_node = checked_malloc(sizeof(list_el_t));
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

void append_head(list_t* list, void* value) {
	list_el_t* new_node = checked_malloc(sizeof(list_el_t));
	new_node->current_ele = value;
	new_node->next = list->head; 

	if (list->length == 0) list->head = list->tail = new_node; 
	else list->head = new_node;

	list->length++; 
}

// TODO: recursively free the tree
void free_list(list_t* list) {

	foreach(list) {
		free(curr->current_ele);
		free(curr);
	}

	free(list); 
}

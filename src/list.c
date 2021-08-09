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

/*
int main(int argc, char* argv[]) {
	char* str1 = malloc(sizeof(char) * 5);
	str1 = "john";
	char* str2 = malloc(sizeof(char) * 6);
	str2 = "world"; 

	list_t* l = init_list();
	append(l, str1);
	append(l, str2); 

	list_el_t* curr_node = l->head;
	while (curr_node != NULL) {
		puts(curr_node->current_ele);
		curr_node = curr_node->next; 
	}
}
*/ 

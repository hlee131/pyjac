#ifndef LIST_H
#define LIST_H
#include <stddef.h>
typedef struct list_el_s {
	void* current_ele;
	struct list_el_s* next; 
} list_el_t;

typedef struct list_s {
	list_el_t* head;
	list_el_t* tail; 
	size_t length; 
} list_t;

list_t* init_list(); 
void free_list(list_t* list); 
void append(list_t* list, void* value); 
#endif


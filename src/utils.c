#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "includes/ast.h"

// checked malloc function 
void* checked_malloc(size_t size) {
	void* ptr = malloc(size); 
	if (ptr) return ptr;
	else {
		printf("failed to malloc of size %zu\n", size);
		exit(EXIT_FAILURE); 
	}
}

// substring function
char* substring(char* start, size_t len) {
	char* substr = checked_malloc(sizeof(char) * (len + 1)); 
	memcpy(substr, start, len);
	substr[len] = 0;
	return substr; 
}

// returns a string describing a type
char* type_str(type_node_t* type) {
	char* str; 
	switch (type->type) {
		case INT_T: str = "Int"; break; 
		case STR_T: str = "Str"; break; 
		case BOOL_T: str = "Bool"; break;
		case DOUBLE_T: str = "Double"; break;  
	}

	char* full_str = checked_malloc((strlen(str) + (4 * type->arr_count) + 1) * sizeof(char));

	if (type->arr_count) {
		for (int i = 0; i < type->arr_count; i++) strcpy(full_str + (i * 4), "Arr:"); 
		strcpy(full_str + (type->arr_count * 4), str);
	} else strcpy(full_str, str); 

	full_str[strlen(str) + (4 * type->arr_count)] = 0; 
	return full_str; 
}

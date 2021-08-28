#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h> 
#include "ast.h"

char* substring(char* start, size_t len);
void* checked_malloc(size_t size); 
char* type_str(type_node_t* type);

#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// substring function
char* substring(char* start, size_t len) {
	char* substr = malloc(sizeof(char) * (len + 1)); 
	memcpy(substr, start, len);
	substr[len] = 0;
	return substr; 
}

// checked malloc function 
void* checked_malloc(size_t size) {
	void* ptr = malloc(size); 
	if (ptr) return ptr;
	else {
		printf("failed to malloc of size %zu\n", size);
		exit(1); 
	}
}

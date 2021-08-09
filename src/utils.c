#include <stdlib.h>

// substring function
char* substring(char* start, size_t len) {
	char* substr = malloc(sizeof(char) * (length + 1)); 
	memcpy(substr, start, len);
	substr[len] = 0;
	return substr; 
}

// checked malloc function 

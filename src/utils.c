#include <string.h>

// substring function
char* substring(char* start, size_t len) {
	char* substr = malloc(sizeof(char) * (len + 1)); 
	memcpy(substr, start, len);
	substr[len] = 0;
	return substr; 
}

// checked malloc function 

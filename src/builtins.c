#include <stdio.h>

// basic builtin functions 
// to be expanded in the future
int show(int num) {
	return printf("%d\n", num); 
}

int max(int x, int y) {
	return x > y ? x : y; 
}

int min(int x, int y) {
	return x < y ? x : y; 
}

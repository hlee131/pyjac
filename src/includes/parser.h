#ifndef PARSER_H
#define PARSER_H
#include "list.h" 
#include "lexer.h" 
typedef struct parser_s {
	token_stream_t* token_stream; 
	list_t* ast; 
} parser_t;

parser_t* init_parser(char* src); 
#endif

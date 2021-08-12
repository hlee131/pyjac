#include <stdlib.h>

#include "includes/parser.h" 
#include "includes/lexer.h"
#include "includes/list.h" 

parser_t* init_parser(char* src) {
	parser_t* parser = malloc(sizeof(parser_t));
	parser->token_stream = make_stream(src);
	parser->ast = init_list(); 
	return parser; 
}	

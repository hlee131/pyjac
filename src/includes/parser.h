#ifndef PARSER_H
#define PARSER_H
#include "list.h" 
#include "lexer.h"
#include "ast.h" 
typedef struct parser_s {
	token_stream_t* token_stream; 
	list_t* ast; 
} parser_t;

parser_t* init_parser(char* src);
void free_parser(parser_t* p); 
type_node_t* parse_types(token_stream_t* ts);
list_t* parse_params(token_stream_t* ts, int is_formal_params); 
state_ast_t* parse_function(token_stream_t* ts); 
list_t* parse_block(token_stream_t* ts); 
state_ast_t* parse_ret(token_stream_t* ts);
state_ast_t* parse_expr_state(token_stream_t* ts);
state_ast_t* parse_if(token_stream_t* ts); 
state_ast_t* parse_while(token_stream_t* ts); 
state_ast_t* parse_decl(token_stream_t* ts);
int expect(tok_type_t expected, token_stream_t* ts);
expr_ast_t* parse_expression(token_stream_t* ts); 
#endif

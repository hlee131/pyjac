#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h> 

#include "list.h" 
#include "lexer.h"
#include "ast.h" 

typedef struct parser_s {
	token_stream_t* token_stream; 
	prog_ast_t ast; 
} parser_t;

parser_t* init_parser(char* src);
void free_parser(parser_t* p); 
void parse_program(parser_t* p);
type_node_t* parse_types(token_stream_t* ts);
list_t* parse_params(token_stream_t* ts, bool is_formal_params); 
state_ast_t* parse_function(token_stream_t* ts); 
block_ast_t parse_block(token_stream_t* ts); 
state_ast_t* parse_statement(token_stream_t* ts);
state_ast_t* parse_ret(token_stream_t* ts);
state_ast_t* parse_expr_state(token_stream_t* ts);
state_ast_t* parse_if(token_stream_t* ts); 
state_ast_t* parse_while(token_stream_t* ts); 
state_ast_t* parse_for(token_stream_t* ts);
state_ast_t* parse_decl(token_stream_t* ts);
expr_ast_t* nud(token_stream_t* ts);
expr_ast_t* led(expr_ast_t* left, token_stream_t* ts);
int bp(tok_type_t tok);
int expect(tok_type_t expected, token_stream_t* ts);
expr_ast_t* parse_expression(token_stream_t* ts, int rbp); 
#endif

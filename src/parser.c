#include <stdlib.h>
#include <stdio.h> 

#include "includes/parser.h" 
#include "includes/lexer.h"
#include "includes/list.h" 
#include "includes/ast.h" 

#define peek(ts) (ts->stream[ts->stream_pos+1])
#define adv(ts) (ts->stream_pos++) 

parser_t* init_parser(char* src) {
	parser_t* parser = malloc(sizeof(parser_t));
	parser->token_stream = make_stream(src);
	parser->ast = init_list(); 
	return parser; 
}

void parse_program(parser_t* p) {
	
	token_stream_t* ts = p->token_stream; 

	// Program can only have functions at the top level for now
	while (ts->stream_pos < ts->stream_len) {
		append(p->ast, parse_function(ts)); 
	}
}

// parsing statements

state_ast_t* parse_function(token_stream_t* ts) {
	expect(FUNC_TOK, ts);
	list_t* id = parse_params(ts); 
	expect(L_PAREN_TOK, ts);

}

list_t* parse_block(token_stream_t* ts) {
}

state_ast_t* parse_if(token_stream_t* ts) {
}

state_ast_t* parse_while(token_stream_t* ts) {
}

state_ast_t* parse_for(token_stream_t* ts) {
}

// handle both declaration and assignment? 
state_ast_t* parse_var(token_stream_t* ts) {
}

// parsing expressions

expr_ast_t* parse_bool(token_stream_t* ts) {
}

expr_ast_t* parse_index(token_stream_t* ts) {
}

expr_ast_t* parse_call(token_stream_t* ts) {
}

expr_ast_t* parse_binop(token_stream_t* ts) {
}

int expect(tok_type_t expected, token_stream_t* ts) {
	if (ts->stream[ts->stream_pos].tok_type == expected) return 1; 

	printf("");
}




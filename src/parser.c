#include <stdlib.h>
#include <stdio.h> 

#include "includes/parser.h" 
#include "includes/lexer.h"
#include "includes/list.h" 
#include "includes/ast.h" 

#define peek(ts) (ts->stream[ts->stream_pos+1])
#define adv(ts) (ts->stream_pos++) 
#define curr(ts) (ts->stream[ts->stream_pos]) 

parser_t* init_parser(char* src) {
	parser_t* parser = malloc(sizeof(parser_t));
	parser->token_stream = make_stream(src);
	parser->ast = init_list(); 
	return parser; 
}

void free_parser(parser_t* p) {
	free_stream(p->token_stream);
	free_list(p->ast); 
	free(p); 
}

// parsing programs 

void parse_program(parser_t* p) {
	
	token_stream_t* ts = p->token_stream; 

	// Program can only have functions at the top level for now
	while (ts->stream_pos < ts->stream_len) {
		append(p->ast, parse_function(ts)); 
	}
}


// parsing misc phrases

type_node_t* parse_types(token_stream_t* ts) {
	int arr_count = 0;
	int type;
	int accepting_arr = 1; 

	while (1) {
		switch (curr(ts).tok_type) {
			case INT_T_TOK:
				accepting_arr = 0; 
				type = INT_T; goto EXIT; 
			case STR_T_TOK:
				accepting_arr = 0; 
				type = STR_T; goto EXIT; 
			case DOUBLE_T_TOK:
				accepting_arr = 0;
				type = DOUBLE_T; goto EXIT; 
			case BOOL_T_TOK:
				accepting_arr = 0;
				type = BOOL_T; goto EXIT; 
			case ARR_T_TOK: 
				if (!accepting_arr) {
					// TODO: print formatting error message
				} else arr_count++; 
			default: goto EXIT;  
		}
		adv(ts); 
		expect(COLON_TOK, ts); 
	}

	EXIT:
	if (!type) {} // TODO: print some error message  
	else {
		expect(COLON_TOK, ts); 
		return type_node(type, arr_count); 
	}
}


list_t* parse_params(token_stream_t* ts) {
	
	list_t* params = init_list(); 

	while (peek(ts).tok_type != R_PAREN_TOK) {
		type_node_t* param_type = parse_types(ts); 
		char* param_name = curr(ts).tok_val;
		append(params, id_ast(param_name, param_type)); 

		adv(ts); 
		expect(COMMA_TOK, ts); 
	}

	return params; 

}

// parsing statements

state_ast_t* parse_function(token_stream_t* ts) {
	
	int line = curr(ts).line; 
	int pos = curr(ts).pos; 

	expect(FUNC_TOK, ts); 
	type_node_t* type = parse_types(ts); 
	expect(L_PAREN_TOK, ts);
	list_t* params = parse_params(ts); 
	expect(R_PAREN_TOK, ts); 
	expect(ARROW_TOK, ts); 
	
	id_ast_t* func_type = id_ast(curr(ts).tok_val, type); 	
	adv(ts); 
	list_t* content = parse_block(ts); 
	
	return func_ast(func_type, params, content, line, pos); 
}

list_t* parse_block(token_stream_t* ts) {
	list_t* statements = init_list(); 
	expect(INDENT_TOK, ts);
	// do some parsing in here
	expect(DEDENT_TOK, ts); 	
}


state_ast_t* parse_if(token_stream_t* ts) {
	list_t* if_pairs = init_list(); 
	int line = curr(ts).line;
	int pos = curr(ts).pos; 
	expect(IF_TOK, ts);

	do {
		expect(L_PAREN_TOK, ts);
		// TODO: write parse_expression
		expr_ast_t* condition = parse_expression(ts); 
		expect(R_PAREN_TOK, ts); 
		expect(COLON_TOK, ts); 
		list_t* block = parse_block(ts);
		append(if_pairs, if_pair(condition, block)); 

	} while (curr(ts).tok_type == ELIF_TOK);

	if (curr(ts).tok_type == ELSE_TOK) {
		expr_ast_t* condition = bool_node(1, curr(ts).line, curr(ts).pos); 
		adv(ts);
		expect(COLON_TOK, ts); 
		list_t* block = parse_block(ts); 
		append(if_pairs, if_pair(condition, block)); 
	}	

	return if_ast(if_pairs, line, pos); 
}

 
state_ast_t* parse_while(token_stream_t* ts) {
	
	int line = curr(ts).line;
	int pos = curr(ts).pos; 
	expect(WHILE_TOK, ts); 
	expect(L_PAREN_TOK, ts);
	expr_ast_t* condition = parse_expression(ts); 
	expect(R_PAREN_TOK, ts); 
	expect(COLON_TOK, ts);
	list_t* block = parse_block(ts);
	return while_ast(condition, block, line, pos); 

}

/* 
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
*/
int expect(tok_type_t expected, token_stream_t* ts) {
	if (ts->stream[ts->stream_pos].tok_type == expected) {
		adv(ts); 
		return 1; 
	}

	printf("");
}




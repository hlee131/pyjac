#include <stdlib.h>
#include <stdio.h> 
#include <stdbool.h>

#include "includes/parser.h" 
#include "includes/lexer.h"
#include "includes/list.h" 
#include "includes/ast.h" 
#include "includes/utils.h"

#define peek(ts) (ts->stream[ts->stream_pos+1])
#define adv(ts) (ts->stream_pos++) 
#define curr(ts) (ts->stream[ts->stream_pos]) 

parser_t* init_parser(char* src) {
	parser_t* parser = (parser_t*) checked_malloc(sizeof(parser_t));
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
	// TODO: find better way of dealing with END_TOKs
	while (curr(ts).tok_type == END_TOK) adv(ts);
	while (curr(ts).tok_type == FUNC_TOK || curr(ts).tok_type == END_TOK) {
		if (curr(ts).tok_type == FUNC_TOK) append(p->ast, parse_function(ts));
		else adv(ts);
	}

	printf("parser: finished parsing with %zu functions\n", p->ast->length);
}


// parsing misc phrases
type_node_t* parse_types(token_stream_t* ts) {
	int arr_count = 0;
	int type; 

	while (1) {
		switch (curr(ts).tok_type) {
			case INT_T_TOK: type = INT_T; goto EXIT; 
			case STR_T_TOK: type = STR_T; goto EXIT; 
			case DOUBLE_T_TOK: type = DOUBLE_T; goto EXIT; 
			case BOOL_T_TOK: type = BOOL_T; goto EXIT; 
			case ARR_T_TOK: 
				arr_count++; break; 
			default: goto EXIT;  
		}
		adv(ts); 
		expect(COLON_TOK, ts); 
	}

	EXIT:
		adv(ts);
		if (!type) {} // TODO: print some error message  
		else {
			expect(COLON_TOK, ts); 
			return type_node(type, arr_count); 
		}
}


list_t* parse_params(token_stream_t* ts, bool is_formal_params) {
	
	list_t* params = init_list(); 

	while (curr(ts).tok_type != R_PAREN_TOK && 
		   curr(ts).tok_type != R_CURL_TOK) {
		if (is_formal_params) {
			type_node_t* param_type = parse_types(ts); 
			char* param_name = curr(ts).tok_val;
			append(params, id_ast(param_name, param_type)); 
			adv(ts); 
		} else {
			append(params, parse_expression(ts, 0)); 
		}

		if (curr(ts).tok_type != R_PAREN_TOK && 
		   curr(ts).tok_type != R_CURL_TOK) expect(COMMA_TOK, ts); 
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
	params_ast_t params = parse_params(ts, true); 
	expect(R_PAREN_TOK, ts); 
	expect(ARROW_TOK, ts); 
	
	id_ast_t* func_type = id_ast(curr(ts).tok_val, type); 	
	adv(ts); expect(COLON_TOK, ts);
	block_ast_t content = parse_block(ts); 
	
	return func_ast(func_type, params, content, line, pos); 
}

// valid statement is also an expression + end_tok
block_ast_t parse_block(token_stream_t* ts) {
	block_ast_t statements = init_list(); 
	expect(INDENT_TOK, ts);
	while (curr(ts).tok_type != DEDENT_TOK) {
		append(statements, parse_statement(ts));
	}
	expect(DEDENT_TOK, ts); 	
	return statements; 
}

state_ast_t* parse_statement(token_stream_t* ts) {
	switch (curr(ts).tok_type) {
		case IF_TOK: return parse_if(ts);
		case FOR_TOK: return parse_for(ts);
		case WHILE_TOK: return parse_while(ts);
		case VAR_TOK: {
			state_ast_t* ast = parse_decl(ts);
			if (curr(ts).tok_type != DEDENT_TOK) expect(END_TOK, ts);
			return ast; 
		}
		case RET_TOK: {
			state_ast_t* ast = parse_ret(ts);
			if (curr(ts).tok_type != DEDENT_TOK) expect(END_TOK, ts);
			return ast; 
		}
		case END_TOK: adv(ts); return parse_statement(ts);
		default: {
			state_ast_t* ast = parse_expr_state(ts);
			if (curr(ts).tok_type != DEDENT_TOK) expect(END_TOK, ts);
			return ast; 
		}
	}
}

state_ast_t* parse_expr_state(token_stream_t* ts) {
	int line = curr(ts).line;
	int pos = curr(ts).pos;
	expr_ast_t* expr = parse_expression(ts, 0);
	return expr_ast(expr, line, pos);
}

state_ast_t* parse_ret(token_stream_t* ts) {
	int line = curr(ts).line;
	int pos = curr(ts).pos;
	expect(RET_TOK, ts);
	state_ast_t* ast = ret_ast(parse_expression(ts, 0), line, pos); 
	return ast; 
}


state_ast_t* parse_if(token_stream_t* ts) {
	if_ast_t if_pairs = init_list(); 
	int line = curr(ts).line;
	int pos = curr(ts).pos; 
	expect(IF_TOK, ts);

	expect(L_PAREN_TOK, ts);
	expr_ast_t* condition = parse_expression(ts, 0); 
	expect(R_PAREN_TOK, ts); 
	expect(COLON_TOK, ts); 
	block_ast_t block = parse_block(ts);
	append(if_pairs, if_pair(condition, block)); 

	while (curr(ts).tok_type == ELIF_TOK) {
		expect(ELIF_TOK, ts); 
		expect(L_PAREN_TOK, ts);
		expr_ast_t* condition = parse_expression(ts, 0); 
		expect(R_PAREN_TOK, ts); 
		expect(COLON_TOK, ts); 
		block_ast_t block = parse_block(ts);
		append(if_pairs, if_pair(condition, block));
	}

	if (curr(ts).tok_type == ELSE_TOK) {
		expr_ast_t* condition = bool_node(1, curr(ts).line, curr(ts).pos); 
		adv(ts);
		expect(COLON_TOK, ts); 
		block_ast_t block = parse_block(ts); 
		append(if_pairs, if_pair(condition, block)); 
	}	

	return if_ast(if_pairs, line, pos); 
}

 
state_ast_t* parse_while(token_stream_t* ts) {
	int line = curr(ts).line;
	int pos = curr(ts).pos; 
	expect(WHILE_TOK, ts); 
	expect(L_PAREN_TOK, ts);
	expr_ast_t* condition = parse_expression(ts, 0); 
	expect(R_PAREN_TOK, ts); 
	expect(COLON_TOK, ts);
	block_ast_t block = parse_block(ts);
	return while_ast(condition, block, line, pos); 
}

 
state_ast_t* parse_for(token_stream_t* ts) {
	int line = curr(ts).line;
	int pos = curr(ts).pos;
	expect(FOR_TOK, ts);
	expect(L_PAREN_TOK, ts);
	state_ast_t* initializer = parse_decl(ts);
	expect(VERT_TOK, ts); 
	expr_ast_t* condition = parse_expression(ts, 0);
	expect(VERT_TOK, ts);
	expr_ast_t* updater = parse_expression(ts, 0);
	expect(R_PAREN_TOK, ts);
	expect(COLON_TOK, ts);
	block_ast_t block = parse_block(ts); 
	return for_ast(initializer, condition, updater, block, line, pos);
}

// handle both declaration and assignment? 
state_ast_t* parse_decl(token_stream_t* ts) {
	int line = curr(ts).line;
	int pos = curr(ts).pos;
	expect(VAR_TOK, ts);
	type_node_t* type = parse_types(ts);
	id_ast_t* id = id_ast(curr(ts).tok_val, type);
	adv(ts);
	expect(ASSIGN_TOK, ts);
	expr_ast_t* expr = parse_expression(ts, 0);
	return decl_ast(id, expr, line, pos); 
}

// parsing expressions
// returns the binding power of an operator
int bp(tok_type_t tok) {
	switch (tok) {
		case PLUS_TOK:
		case MINUS_TOK:
			return 30;
		case MUL_TOK:
		case DIV_TOK:
			return 40;
		case EQUALS_TOK:
		case LESS_EQUAL_TOK: 
		case GREAT_EQUAL_TOK:
		case NOT_EQUAL_TOK: 
		case LESS_TOK:
		case GREAT_TOK:
			return 20;
		case ASSIGN_TOK:
			return 10;
		default: return 0; 
	}
}

// For information about Pratt parsing:
// https://tdop.github.io/
// rbp stands for right binding power
expr_ast_t* parse_expression(token_stream_t* ts, int rbp) {
	expr_ast_t* left = nud(ts);
	while ((
		curr(ts).tok_type == PLUS_TOK ||
		curr(ts).tok_type == MINUS_TOK ||
		curr(ts).tok_type == MUL_TOK ||
		curr(ts).tok_type == DIV_TOK ||
		curr(ts).tok_type == EQUALS_TOK ||
		curr(ts).tok_type == LESS_EQUAL_TOK ||
		curr(ts).tok_type == GREAT_EQUAL_TOK ||
		curr(ts).tok_type == NOT_EQUAL_TOK ||
		curr(ts).tok_type == LESS_TOK ||
		curr(ts).tok_type == GREAT_TOK ||
		curr(ts).tok_type == ASSIGN_TOK 
	) && bp(curr(ts).tok_type) > rbp ) 
	{ left = led(left, ts); }
	
	return left; 
}

// Left-denotation
expr_ast_t* led(expr_ast_t* left, token_stream_t* ts) {
	int type; 
	int line = curr(ts).line;
	int pos = curr(ts).pos;
	int binding = bp(curr(ts).tok_type);

	switch (curr(ts).tok_type) {
		case PLUS_TOK: type = ADD_NODE; break;
		case MINUS_TOK: type = SUB_NODE; break;
		case MUL_TOK: type = MUL_NODE; break;
		case DIV_TOK: type = DIV_NODE; break;
		case EQUALS_TOK: type = EQ_NODE; break;
		case LESS_EQUAL_TOK: type = LE_NODE; break;
		case GREAT_EQUAL_TOK: type = GE_NODE; break;
		case NOT_EQUAL_TOK: type = NEQ_NODE; break;
		case LESS_TOK: type = LT_NODE; break;
		case GREAT_TOK: type = GT_NODE; break;
		case ASSIGN_TOK: type = ASSIGN_NODE; break;
		default: break; 
	}

	adv(ts); 
	return binop_ast(type, left, parse_expression(ts, binding), line, pos); 
}

// Null-detonation 
expr_ast_t* nud(token_stream_t* ts) {
	expr_ast_t* ast; 
	int line = curr(ts).line;
	int pos = curr(ts).pos;

	switch(curr(ts).tok_type) {
		case INT_L_TOK: 
			ast = int_node(atoi(curr(ts).tok_val), line, pos);
			break; 
		case STR_L_TOK: 
			ast = str_node(curr(ts).tok_val, line, pos);
			break; 
		case DOUBLE_L_TOK:
			ast = double_node(atof(curr(ts).tok_val), line, pos);
			break; 
		case ID_L_TOK:
			ast = id_node(curr(ts).tok_val, line, pos);
			switch (peek(ts).tok_type) {
				case L_CURL_TOK:
					adv(ts); adv(ts); 
					ast = binop_ast(INDEX_NODE, ast, parse_expression(ts, 0), line, pos);
					expect(R_CURL_TOK, ts);
					return ast; 
				case L_PAREN_TOK:
					adv(ts); adv(ts);
					char* func_name = ast->children.str_val; free(ast); 
					ast = call_ast(func_name, parse_params(ts, false), line, pos);
					expect(R_PAREN_TOK, ts);
					return ast; 
				default: break; 
			}
			break; 
		case TRUE_TOK:
			ast = bool_node(1, line, pos); break; 
		case FALSE_TOK:
			ast = bool_node(0, line, pos); break;
		case L_PAREN_TOK:
			adv(ts);
			ast = parse_expression(ts, 0); 
			expect(R_PAREN_TOK, ts);
		default: break; 
	}
	adv(ts); 
	return ast; 
}

int expect(tok_type_t expected, token_stream_t* ts) {
	if (curr(ts).tok_type == expected) {
		adv(ts); 
		return 1; 
	}

	int error_line = curr(ts).line;
	int error_pos = curr(ts).pos; 
	char* expect = tok_string(expected);
	char* actual = tok_string(curr(ts).tok_type);

	// panic mode for error handling and recovery
	while (curr(ts).tok_type != R_PAREN_TOK &&
		   curr(ts).tok_type != R_CURL_TOK &&
		   curr(ts).tok_type != END_TOK &&
		   curr(ts).tok_type != DEDENT_TOK) adv(ts); 

	printf("PARSE ERROR: line %d, pos %d, expected %s but got %s (panic mode jumped to line %d, pos %d)\n", 
		error_line, error_pos, expect, actual, curr(ts).line, curr(ts).pos); 

	free(expect); free(actual);
	
	return 0;
}

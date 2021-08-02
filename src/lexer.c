#include <stdlib.h>
#include <stdio.h> 

#include "includes/lexer.h" 

Lexer* init_lexer(char* src) {
	
	// init lexer
	Lexer* l = calloc(1, sizeof(Lexer));
	l->src = src;
	l->tok_len = 0;
	l->pos = l->line = 1;
	l->ident_level = 0; 

	// init token 
	l->curr_tok.tok_start = src;
	l->curr_tok.tok_len = 0;
	l->curr_tok.tok_type = NULL_TOK;

	return l; 
}

void next(Lexer* lexer) {
	
	// check if end of source
	if (*(lexer->src) = 0) lexer->curr_tok.tok_type = NULL_TOK; 

	// increment past any spaces
	while (*(lexer->src) == ' ') lexer->src++; lexer->pos++;  

	// skip any comments
	if (*(lexer->src) == '[') {
		puts("COMMENT DETECTED, SKIPPING"); 
		while (*(lexer->src) != ']') {
			lexer->src++;
			if (*(lexer->src) == '\n') {
				lexer->line++; 
				lexer->pos = 0;
			} else lexer->pos++; 
		}
		puts("CONTINUIING NORMALLY"); 
	}

	switch (*(lexer->src)) {
		case '\n': detect_whitespace(lexer); break; 
		case '+':
			if (lexer->src[1] == '+') {
				lexer->curr_tok.tok_type = INCR_TOK; 
				lexer->pos++; lexer->src++;
			} else lexer->curr_tok.tok_type = PLUS_TOK; 
			break; 
		case '-':
			if (lexer->src[1] == '-') {
				lexer->curr_tok.tok_type = DECR_TOK; 
				lexer->pos++; lexer->src++;
			} else lexer->curr_tok.tok_type = MINUS_TOK; 
			break; 
		case '*': lexer->curr_tok.tok_type = MUL_TOK; break; 
		case '/': lexer->curr_tok.tok_type = DIV_TOK; break; 
		case '<': 
			if (lexer->src[1] == '=') {
				lexer->curr_tok.tok_type = LESS_EQUAL_TOK;
				lexer->pos++; lexer->src++; 
			} else lexer->curr_tok.tok_type = LESS_TOK; 
			break; 
		case '>':
			if (lexer->src[1] == '=') {
				lexer->curr_tok.tok_type = GREAT_EQUAL_TOK;
				lexer->pos++; lexer->src++; 
			} else lexer->curr_tok.tok_type = GREAT_TOK; 
			break; 
		case '!':
			if (lexer->src[1] == '=') lexer->curr_tok.tok_type = NOT_EQUAL_TOK;  
			else {
				printf("line %d, pos %d: expected '=' but received '%c'",
					lexer->line, lexer->pos, lexer->src[1]); 
			}
			lexer->pos++; lexer->src++; break; 
		case '=':
			switch (lexer->src[1]) {
				case '>':
					lexer->curr_tok.tok_type = ARROW_TOK;
					lexer->pos++; lexer->src++; break; 
				case '=':
					lexer->curr_tok.tok_type = EQUALS_TOK;
					lexer->pos++; lexer->src++; break; 
				default: lexer->curr_tok.tok_type = ASSIGN_TOK; break; 
			} break; 
		case '{': lexer->curr_tok.tok_type = L_CURL_TOK; break; 
		case '}': lexer->curr_tok.tok_type = R_CURL_TOK; break; 
		case '(': lexer->curr_tok.tok_type = L_PAREN_TOK; break;
		case ')': lexer->curr_tok.tok_type = R_PAREN_TOK; break; 
		case ':': lexer->curr_tok.tok_type = COLON_TOK; break; 
		case '|': lexer->curr_tok.tok_type = VERT_TOK; break; 
		default:
			printf("line %d, pos %d: invalid character '%c'", 
				lexer->line, lexer->pos, *(lexer->src));
			lexer->pos++; lexer->line++; break; 
	}
	
	lexer->src++; 
}

// decides whether to emit ident, dedent, or end token
void detect_whitespace(Lexer* lexer) {
	
	int tab_level = 0; 
	
	while (lexer->src[1] == '\t') {
		tab_level++; lexer->pos++; lexer->src++; 
	}

	if (tab_level == lexer->ident_level) lexer->curr_tok.tok_type = END_TOK;
	else if (tab_level > lexer->ident_level) lexer->curr_tok.tok_type = INDENT_TOK;
	else if (tab_level < lexer->ident_level) lexer->curr_tok.tok_type = DEDENT_TOK; 

}

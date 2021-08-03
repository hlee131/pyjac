#include <stdlib.h>
#include <stdio.h> 
#include <ctype.h>
#include <string.h> 

#include "includes/lexer.h" 

#define len(x) (sizeof(x) / sizeof(x[0]))

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

void free_lexer(Lexer* lexer) {
	free(lexer->src);
	free(lexer); 
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
		case '\n': lex_whitespace(lexer); break; 
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

int is_keyword(Lexer* lexer) {

	const struct pair {
		char* keyword;
		TokenType keyword_type;
	} keywords[] = {
		{"Int", INT_T_TOK}, {"Str", STR_T_TOK}, {"Double", DOUBLE_T_TOK},
		{"Bool", BOOL_T_TOK}, {"Arr", ARR_T_TOK}, {"TRUE", TRUE_TOK},
		{"FALSE", FALSE_TOK}, {"func", FUNC_TOK}, {"ret", RET_TOK}, 
		{"if", IF_TOK}, {"while", WHILE_TOK}, {"for", FOR_TOK},
		{"var", VAR_TOK} 
	};

	for (int i = 0; i < len(keywords); i++) {
		if (memcmp(keywords[i].keyword, 
			lexer->curr_tok.tok_start, 
			lexer->curr_tok.tok_len * sizeof(char))) {
			
			lexer->curr_tok.tok_type = keywords[i].keyword_type; 
			return 1; 
		}
	}

	return 0; 
	
}

// lex tokens that contain characters and numbers, i.e. literals, types, booleans,
// and keywords
// returns 1 if a token is identified else 0 
int lex_alnum(Lexer* lexer) {

	if (*(lexer->src) == '"') {
		// Read string literal into lexer state
		lexer->curr_tok.tok_start = ++(lexer->src);
		lexer->curr_tok.tok_type = STR_L_TOK;

		while (*(lexer->src) != '"' && *(lexer->src) != 0) {
			lexer->curr_tok.tok_len++;
			lexer->src++; lexer->pos++; 
		}
		
		lexer->src++; lexer->pos++; 
	} else if (isalpha(*(lexer->src))) {

		// Read identifier or keyword into lexer state
		lexer->curr_tok.tok_start = lexer->src;

		while (isalnum(*(lexer->src)) || *(lexer->src) == '_') {
			lexer->curr_tok.tok_len++;
			lexer->src++; lexer->pos++; 
		}

		if (!is_keyword(lexer)) lexer->curr_tok.tok_type = ID_L_TOK; 

	} else if (isdigit(*(lexer->src))) {

		// Read double or integer into lexer state
		lexer->curr_tok.tok_start = lexer->src;
		int decimal_used = 0; 

		while (isdigit(*(lexer->src)) && (*(lexer->src) == '.' ^ decimal_used)) {
			if (*(lexer->src) == '.') decimal_used = 1; 
			lexer->curr_tok.tok_len++;
			lexer->src++; lexer->pos++; 
		}
		
		lexer->curr_tok.tok_type = decimal_used ? DOUBLE_L_TOK : INT_L_TOK;

	} else return 0; 

}

// decides whether to emit ident, dedent, or end token
void lex_whitespace(Lexer* lexer) {
	
	int tab_level = 0; 
	
	while (lexer->src[1] == '\t') {
		tab_level++; lexer->pos++; lexer->src++; 
	}

	if (tab_level == lexer->ident_level) lexer->curr_tok.tok_type = END_TOK;
	else if (tab_level > lexer->ident_level) lexer->curr_tok.tok_type = INDENT_TOK;
	else if (tab_level < lexer->ident_level) lexer->curr_tok.tok_type = DEDENT_TOK;

	lexer->ident_level = tab_level; 	

}

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
	l->indent_stack[0] = 0; 
	l->stack_index = 1; 
	l->emit_dedent_count = 0; 

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

void cleanup_whitespace(Lexer* lexer) {
	lexer->emit_dedent_count = lexer->stack_index - 1;
	printf("%d ", lexer->stack_index); 
	lexer->stack_index = 1; 
}

void next(Lexer* lexer) {

	// checks if there are any dedents we need to emit
	if (lexer->emit_dedent_count > 0) {
		lexer->emit_dedent_count--; 
		lexer->curr_tok.tok_type = DEDENT_TOK; 
	}

	// reset previous token state
	lexer->curr_tok.tok_len = 0; 
	
	// check if end of source
	if (*(lexer->src) == 0) { 
		cleanup_whitespace(lexer); 
		if (lexer->emit_dedent_count == 0) {
			lexer->curr_tok.tok_type = NULL_TOK;	
		} else {
			lexer->emit_dedent_count--;
			lexer->curr_tok.tok_type = DEDENT_TOK; 
		}
		return; 
	}

	// increment past any spaces
	while (*(lexer->src) == ' ') { lexer->src++; lexer->pos++; }

	// skip any comments
	if (*(lexer->src) == '[') {
		puts("COMMENT DETECTED, SKIPPING"); 
		while (*(lexer->src) != ']') {
			if (*(lexer->src) == 0) {
				printf("line %d, pos %d: expected ']' to end comment but received end of file\n",
					lexer->line, lexer->pos); 
				lexer->curr_tok.tok_type = NULL_TOK; return; 
			}

			lexer->src++;
			if (*(lexer->src) == '\n') {
				lexer->line++; 
				lexer->pos = 0;
			} else lexer->pos++; 
		}
		lexer->src++; lexer->pos++; 
	}

	switch (*(lexer->src)) {
		case '\n': lex_whitespace(lexer); lexer->line++; break; 
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
		case ',': lexer->curr_tok.tok_type = COMMA_TOK; break;  
		default:
			if (!lex_alnum(lexer)) {
				printf("line %d, pos %d: invalid character '%c'", 
					lexer->line, lexer->pos, *(lexer->src));
			}
	}
	
	lexer->src++; lexer->pos++; 
}

int is_keyword(Lexer* lexer) {

	/*
	char string[lexer->curr_tok.tok_len+1];
	string[lexer->curr_tok.tok_len] = 0;
	memcpy(string, lexer->curr_tok.tok_start, lexer->curr_tok.tok_len);
	puts(string); 
	*/

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
			lexer->curr_tok.tok_len * sizeof(char)) == 0 
		&& strlen(keywords[i].keyword) == lexer->curr_tok.tok_len) {
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
		
		return 1; 

	} else if (isalpha(*(lexer->src))) {

		// Read identifier or keyword into lexer state
		lexer->curr_tok.tok_start = lexer->src;

		while (isalnum(*(lexer->src)) || *(lexer->src) == '_') {
			lexer->curr_tok.tok_len++;
			lexer->src++; lexer->pos++; 
		}

		lexer->src--; lexer->pos--; 

		if (!is_keyword(lexer)) lexer->curr_tok.tok_type = ID_L_TOK; 
		return 1; 

	} else if (isdigit(*(lexer->src))) {
		// Read double or integer into lexer state
		lexer->curr_tok.tok_start = lexer->src;
		int decimal_used = 0; 

		while (isdigit((lexer->src)[1]) || ((lexer->src)[1] == '.')) {
			lexer->curr_tok.tok_len++;
			lexer->src++; lexer->pos++; 
			if (*(lexer->src) == '.' && decimal_used) {
				lexer->src--; lexer->pos--; break; 
			} else  decimal_used = *(lexer->src) == '.'; 
		}

		lexer->curr_tok.tok_type = decimal_used ? DOUBLE_L_TOK : INT_L_TOK;

		return 1; 
	} else return 0; 

}

// decides whether to emit ident, dedent, or end token
void lex_whitespace(Lexer* lexer) {
	
	// clean up whitespace when done reading file
	int curr_level = 0; 
	int lex_level = lexer->indent_stack[lexer->stack_index-1]; 
	
	while (lexer->src[1] == '\t') {
		curr_level++; lexer->pos++; lexer->src++; 
	}
	
	// check if blank line 
	if (lexer->src[1] == '\n') { lexer->pos++; lexer->src++; lex_whitespace(lexer); } 
	else {
		// similar to cpython's implementation with an indent stack 
		if (curr_level == lex_level) { lexer->curr_tok.tok_type = END_TOK; return; }
		else if (curr_level > lex_level) { 
		
			lexer->curr_tok.tok_type = INDENT_TOK; 
			lexer->indent_stack[lexer->stack_index++] = curr_level; 
			return; 

		} else if (curr_level < lex_level) { 

			while (curr_level != lexer->indent_stack[lexer->stack_index-1]) {
				lexer->emit_dedent_count++; lexer->stack_index--;
				if (lexer->stack_index == 0) {
					printf("line %d, pos %d: bad indentation level\n",
						lexer->line, lexer->pos); 
					break; 
				}
			}	

			// emit one dedent right now
			lexer->emit_dedent_count--; 
			lexer->curr_tok.tok_type = DEDENT_TOK;
			return; 
		}
	}

}

void print_token(Token t) {
	char* enum_strings[] = {
		"INT_L_TOK", "STR_L_TOK", "DOUBLE_L_TOK", "ID_L_TOK",
		"INT_T_TOK", "STR_T_TOK", "DOUBLE_T_TOK", "BOOL_T_TOK",
		"ARR_T_TOK", "TRUE_TOK", "FALSE_TOK", "FUNC_TOK",
		"RET_TOK", "IF_TOK", "WHILE_TOK", "FOR_TOK", "VAR_TOK",
		"END_TOK", "INDENT_TOK", "DEDENT_TOK", "PLUS_TOK", "MINUS_TOK",
		"MUL_TOK", "DIV_TOK", "INCR_TOK", "DECR_TOK", "EQUALS_TOK",
		"LESS_EQUAL_TOK", "GREAT_EQUAL_TOK", "NOT_EQUAL_TOK",
		"LESS_TOK", "GREAT_TOK", "ASSIGN_TOK", "L_CURL_TOK", "R_CURL_TOK",
		"L_PAREN_TOK", "R_PAREN_TOK", "COLON_TOK", "ARROW_TOK", "COMMA_TOK",
		"NULL_TOK", "VERT_TOK"
	};

	printf("current token type is: %s\n", enum_strings[t.tok_type]); 
}

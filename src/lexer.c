#include <stdlib.h>
#include <stdio.h> 
#include <ctype.h>
#include <string.h> 

#include "includes/lexer.h" 
#include "includes/utils.h" 

#define len(x) (sizeof(x) / sizeof(x[0]))

lexer_t* init_lexer(char* src) {
	
	// init lexer
	lexer_t* l = calloc(1, sizeof(lexer_t));
	l->src = l->original_src = src;
	l->pos = l->line = 1;
	l->indent_stack[0] = 0; 
	l->stack_index = 1; 
	l->emit_dedent_count = 0; 

	// init token
	l->curr_tok.tok_type = NULL_TOK;

	return l; 
}

token_stream_t* make_stream(char* src) {

	// init stream 
	token_stream_t* stream = malloc(sizeof(token_stream_t)); 
	stream->stream = malloc(0);
	stream->stream_len = 0;
	stream->stream_pos = 0; 

	lexer_t* lexer = init_lexer(src);

	while (1) {
		next(lexer); 
		if (lexer->curr_tok.tok_type == NULL_TOK) break; 
		stream->stream = realloc(stream->stream, ++(stream->stream_len) * sizeof(token_t)); 
		stream->stream[stream->stream_len-1] = lexer->curr_tok; 
	}

	free_lexer(lexer);
	return stream; 
}

void free_lexer(lexer_t* lexer) {
	free(lexer->original_src);
	free(lexer); 
}

void free_stream(token_stream_t* stream) {
	free(stream->stream);
	free(stream); 
}

void cleanup_whitespace(lexer_t* lexer) {
	lexer->emit_dedent_count = lexer->stack_index - 1;
	lexer->stack_index = 1; 
}

void next(lexer_t* lexer) {
	
	// checks if there are any dedents we need to emit
	if (lexer->emit_dedent_count > 0) {
		lexer->emit_dedent_count--; 
		lexer->curr_tok.tok_type = DEDENT_TOK;
		return; 
	}

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
	
	int len = 1; 
	// TODO: find cleaner way
	// TODO: give dedent/indent/end tok substrings too 
	int found = 1;
	switch (*(lexer->src)) {
		case '\n': lex_whitespace(lexer); lexer->line++; break; 
		case '+':
			if (lexer->src[1] == '+') {
				lexer->curr_tok.tok_type = INCR_TOK; 
				lexer->pos++; lexer->src++; len++; 
			} else lexer->curr_tok.tok_type = PLUS_TOK; 
			break; 
		case '-':
			if (lexer->src[1] == '-') {
				lexer->curr_tok.tok_type = DECR_TOK; 
				lexer->pos++; lexer->src++; len++; 
			} else lexer->curr_tok.tok_type = MINUS_TOK; 
			break; 
		case '*': lexer->curr_tok.tok_type = MUL_TOK; break; 
		case '/': lexer->curr_tok.tok_type = DIV_TOK; break; 
		case '<': 
			if (lexer->src[1] == '=') {
				lexer->curr_tok.tok_type = LESS_EQUAL_TOK;
				lexer->pos++; lexer->src++; len++; 
			} else lexer->curr_tok.tok_type = LESS_TOK; 
			break; 
		case '>':
			if (lexer->src[1] == '=') {
				lexer->curr_tok.tok_type = GREAT_EQUAL_TOK;
				lexer->pos++; lexer->src++; len++; 
			} else lexer->curr_tok.tok_type = GREAT_TOK; 
			break; 
		case '!':
			if (lexer->src[1] == '=') {
				lexer->curr_tok.tok_type = NOT_EQUAL_TOK;  
				len++; 
			} else {
				printf("line %d, pos %d: expected '=' but received '%c'",
					lexer->line, lexer->pos, lexer->src[1]); 
			}
			lexer->pos++; lexer->src++; break; 
		case '=':
			switch (lexer->src[1]) {
				case '>':
					lexer->curr_tok.tok_type = ARROW_TOK;
					lexer->pos++; lexer->src++; len++; break; 
				case '=':
					lexer->curr_tok.tok_type = EQUALS_TOK;
					lexer->pos++; lexer->src++; len++; break; 
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
			found = 0; 
			if (!lex_alnum(lexer)) {
				printf("line %d, pos %d: invalid character '%c'", 
					lexer->line, lexer->pos++, *(lexer->src++));
				next(lexer); 
			} 
	}
	
	lexer->curr_tok.tok_val = found ? substring(lexer->src - (len-1), len) : lexer->curr_tok.tok_val;
	lexer->src++; lexer->pos++; 
}

int is_keyword(lexer_t* lexer) {

	/*
	char string[lexer->curr_tok.tok_len+1];
	string[lexer->curr_tok.tok_len] = 0;
	memcpy(string, lexer->curr_tok.tok_start, lexer->curr_tok.tok_len);
	puts(string); 
	*/

	const struct pair {
		char* keyword;
		tok_type_t keyword_type;
	} keywords[] = {
		{"Int", INT_T_TOK}, {"Str", STR_T_TOK}, {"Double", DOUBLE_T_TOK},
		{"Bool", BOOL_T_TOK}, {"Arr", ARR_T_TOK}, {"TRUE", TRUE_TOK},
		{"FALSE", FALSE_TOK}, {"func", FUNC_TOK}, {"ret", RET_TOK}, 
		{"if", IF_TOK}, {"while", WHILE_TOK}, {"for", FOR_TOK}, {"elif", ELIF_TOK},
		{"else", ELSE_TOK}, {"var", VAR_TOK} 
	};

	for (int i = 0; i < len(keywords); i++) {
		if (strcmp(keywords[i].keyword, lexer->curr_tok.tok_val) == 0) {
			lexer->curr_tok.tok_type = keywords[i].keyword_type; 
			return 1; 
		}
	}

	return 0; 
	
}

// lex tokens that contain characters and numbers, i.e. literals, types, booleans,
// and keywords
// returns 1 if a token is identified else 0 
int lex_alnum(lexer_t* lexer) {

	if (*(lexer->src) == '"') {
		// Read string literal into lexer state
		char* tok_start = ++(lexer->src);
		size_t len = 0; 
		lexer->curr_tok.tok_type = STR_L_TOK;

		while (*(lexer->src) != '"' && *(lexer->src) != 0) {
			len++;
			lexer->src++; lexer->pos++; 
		}

		lexer->curr_tok.tok_val = substring(tok_start, len); 
		
		return 1; 

	} else if (isalpha(*(lexer->src))) {

		// Read identifier or keyword into lexer state
		char* tok_start = lexer->src;
		size_t len = 0; 

		while (isalnum(*(lexer->src)) || *(lexer->src) == '_') {
			len++;
			lexer->src++; lexer->pos++; 
		}

		lexer->src--; lexer->pos--; 
		lexer->curr_tok.tok_val = substring(tok_start, len); 

		if (!is_keyword(lexer)) lexer->curr_tok.tok_type = ID_L_TOK; 

		return 1; 

	} else if (isdigit(*(lexer->src))) {
		// Read double or integer into lexer state
		char* tok_start = lexer->src;
		size_t len = 1; 
		int decimal_used = 0; 

		while (isdigit((lexer->src)[1]) || ((lexer->src)[1] == '.')) {
			len++;
			lexer->src++; lexer->pos++; 
			if (*(lexer->src) == '.') {
				if (decimal_used) { lexer->src--; lexer->pos--; break; }
				else decimal_used = 1;
			} 
		}

		lexer->curr_tok.tok_type = decimal_used ? DOUBLE_L_TOK : INT_L_TOK;
		lexer->curr_tok.tok_val = substring(tok_start, len); 

		return 1; 
	} else return 0; 

}

// decides whether to emit ident, dedent, or end token
void lex_whitespace(lexer_t* lexer) {
	
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
		if (curr_level == lex_level) { lexer->curr_tok.tok_type = END_TOK; }
		else if (curr_level > lex_level) { 
		
			lexer->curr_tok.tok_type = INDENT_TOK; 
			lexer->indent_stack[lexer->stack_index++] = curr_level; 

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
		}

	}

}

void print_token(token_t t) {
	char* enum_strings[] = {
		"INT_L_TOK", "STR_L_TOK", "DOUBLE_L_TOK", "ID_L_TOK",
		"INT_T_TOK", "STR_T_TOK", "DOUBLE_T_TOK", "BOOL_T_TOK",
		"ARR_T_TOK", "TRUE_TOK", "FALSE_TOK", "FUNC_TOK",
		"RET_TOK", "IF_TOK", "WHILE_TOK", "FOR_TOK", "VAR_TOK",
		"ELIF_TOK", "ELSE_TOK",	"END_TOK", "INDENT_TOK", "DEDENT_TOK", 
		"PLUS_TOK", "MINUS_TOK", "MUL_TOK", "DIV_TOK", "INCR_TOK", 
		"DECR_TOK", "EQUALS_TOK", "LESS_EQUAL_TOK", "GREAT_EQUAL_TOK", 
		"NOT_EQUAL_TOK", "LESS_TOK", "GREAT_TOK", "ASSIGN_TOK", 
		"L_CURL_TOK", "R_CURL_TOK", "L_PAREN_TOK", "R_PAREN_TOK", 
		"COLON_TOK", "ARROW_TOK", "COMMA_TOK", "NULL_TOK", "VERT_TOK"
	};

	printf("current token type is: %s\n", enum_strings[t.tok_type]); 
	printf("current token value is: %s\n\n", t.tok_val); 
}

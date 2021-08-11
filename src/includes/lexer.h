#ifndef LEXER_H
#define LEXER_H
#define MAX_INDENT 100 

typedef enum tok_type_e {
	// literals
	INT_L_TOK, STR_L_TOK, DOUBLE_L_TOK, ID_L_TOK, 

	// types 
	INT_T_TOK, STR_T_TOK, DOUBLE_T_TOK, 
	BOOL_T_TOK, ARR_T_TOK,

	// boolean values 
	TRUE_TOK, FALSE_TOK,

	// keywords
	FUNC_TOK, RET_TOK, IF_TOK, WHILE_TOK, FOR_TOK, VAR_TOK, 

	// terminators
	// END_TOK is emitted for a newline of same indentation
	// IDENT_TOK is emitted for newline of increased indentation
	// DEDENT_TOK is emitted for a newline of decreased indentation
	// these tokens will act similarly to curly braces in other
	// languages
	END_TOK, INDENT_TOK, DEDENT_TOK, 

	// arithmetic operator
	PLUS_TOK, MINUS_TOK, MUL_TOK, DIV_TOK,
	INCR_TOK, DECR_TOK,

	// compare 
	EQUALS_TOK, LESS_EQUAL_TOK, GREAT_EQUAL_TOK, NOT_EQUAL_TOK,
	LESS_TOK, GREAT_TOK, 

	// assignment
	ASSIGN_TOK,

	// brackets
	L_CURL_TOK, R_CURL_TOK, L_PAREN_TOK, R_PAREN_TOK,

	// misc tokens 
	// NULL_TOK used for EOF and initial state
	COLON_TOK, ARROW_TOK, COMMA_TOK, NULL_TOK, VERT_TOK
} tok_type_t; 

typedef struct token_s { 
	tok_type_t tok_type; 
	
	int line;
	int pos; 

	// only used for literals 	
	char* tok_val;

} token_t;


typedef struct lexer_s {
	char* original_src; 
	char* src;
	int tok_len; 
	int pos;
	int line;
	int indent_stack[MAX_INDENT]; 
	int stack_index; 
	int emit_dedent_count; 
	token_t curr_tok; 
} lexer_t;

typedef struct token_stream_s {
	token_t* stream;
	size_t stream_len;
	int stream_pos; 
} token_stream_t; 

lexer_t* init_lexer(char* src);
void free_lexer(lexer_t* lexer);
token_stream_t* make_stream(char* src); 
void next(lexer_t* lexer); 
void lex_whitespace(lexer_t* lexer); 
int lex_alnum(lexer_t* lexer); 
int is_keyword(lexer_t* lexer); 
void print_token(token_t t); 
void cleanup_whitespace(lexer_t* lexer); 
#endif 

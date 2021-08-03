typedef enum tt {
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
} TokenType; 

typedef struct t { 
	TokenType tok_type; 

	// only used for literals 	
	char* tok_start;
	int tok_len; 
} Token;


typedef struct l {
	char* src;
	int tok_len; 
	int pos;
	int line;
	int ident_level; 
	Token curr_tok; 
} Lexer;

Lexer* init_lexer(char* src);
void free_lexer(Lexer* lexer); 
void next(Lexer* lexer); 
void lex_whitespace(Lexer* lexer); 
int lex_alnum(Lexer* lexer); 
int is_keyword(Lexer* lexer); 

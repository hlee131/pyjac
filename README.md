## What is pyjac?
Pyjac is a statically typed, compiled language with influence from Python, JavaScript, and C (hence its name). The project is currently still a work in progress. Additionally, this project is intended to be an educational project and should not be used in any production environment. 

## Lexer
The lexer works by switching on the character that the source pointer is on, creating a behavior that is similar to that of a deterministic finite automata (DFA), though not exactly. In each case, the code will consume as many characters as it can legally (maximal munch). For example, if the source pointer is pointing to a digit, the lexer will continue to consume characters as long as the character is a dot or a digit. Depending on whether a dot was consumed, a integer literal or double literal token is returned. 
### Lexing indentation
The lexer tokenizes indentation in a similar way to [cpython's implementation](https://docs.python.org/3/reference/lexical_analysis.html#indentation). It first skips any blank lines. Afterwards, it manipulates the indentation stack depending on the current indentation level. If the current indentation level is equal to the top of the stack, a simple end line token is emitted. If the current indentation level is greater than the top of the stack, the current indentation level is pushed onto the stack and an indent token is emitted. If the current indentation level is less than the top of the stack, the code repeatedly pops the stack while emitting dedent tokens until a matching value to the current indentation level is found. At the end of the file, the code cleans up the stack by emitting a dedent token for every remaining value on the stack.
## Lexer Error Recovery and Handling 
When the lexer sees a character that it does not recognize, the lexer will print out and error message with the location of said character. It will then skip over the unrecognized character until a recognized one is located, i.e. panic mode recovery and handling. 

## Parser
The parser is a simple LL(1) predictive parser (recursive descent with no backtracking) for statements used in combination with a Pratt parser for expressions. The parser produces an abstract syntax tree, rather than a concrete syntax tree, for type checking, optimizations, code generation, etc. 

## Type checking
The first phase of semantic analysis is type checking. The type checker uses a multi-pass traversal of the tree in order to allow features such as out of order functions. In the first pass, the type checker collects all global members, i.e. functions. In the second pass, the type checker checks each individual statement and expression. 

## Future improvements
### Priority 
- parser error recovery and handling
- ternary operator, negation, bitwise logic, shortcut assignment, increment, decrement, negative numbers, skip keyword
### Nice to have
- overload + with array concatentation
- overload comparison for strings
- implicit casts between ints and doubles
- source location structs & imports 
- garbage collection 
- first class functions

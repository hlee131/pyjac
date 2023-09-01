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
The first phase of semantic analysis is type checking. The type checker uses a multi-pass traversal of the tree in order to allow features such as out of order functions. In the first pass, the type checker collects all global members, i.e. functions, and inserts global symbols into a symbol table (implemented as a hash table) that will act as the "base" symbol table. In the second pass, the type checker uses the global symbol table to check each individual statement and expression, inserting and removing symbols where necessary. 

### Type Checking Error Recovery and Handling
In the type checker, error handling is implemented with a `error` subtype. This `error` subtype allows all operations to succeed with the return type of `error`. Using an `error` subtype prevents cascading or compounding type errors within the compiler. Moreover, an `error` subtype allows the type checker to continue checking other unrelated statements and expressions. Type checking the AST is necessary because while LLVM has types, the types are not enforced. 

## Intermediate Code 
After type checking, LLVM IR is generated using the AST. LLVM IR allows for further optimizations. 

## Demo

Notes: 
- `make` generates a compiler called `pcc`
- when compiling with `pcc` the produced executable is called `out` by default

https://github.com/hlee131/pyjac/assets/59949027/868542bc-9fd1-4e6a-a02d-1db299331335


## Improvements Board 
| Immediate Tasks | Eventual Tasks | Low Priority Features | 
|-----------------|----------------|-----------------------|
| Arrays and strings codegen | Parser error recovery | Function and operator overloading |
| Garbage Collection | Tabs can be spaces | Casting |
| ternary operator, negation, bitwise logic, shortcut assignment, negative numbers, skip keyword | Expand runtime library (io, math, etc.) | First class functions |
| | | Lazy evaluation |
| | | Clean up codebase |

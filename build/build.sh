mv *.o build
c++ build/compiler.o build/lexer.o build/ast.o build/parser.o build/list.o build/utils.o build/symtab.o build/codegen.o \
`llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs` -o $1

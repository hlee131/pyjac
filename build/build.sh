gcc `llvm-config --cflags` -c src/compiler.c src/lexer.c src/ast.c src/parser.c src/list.c src/utils.c src/symtab.c src/codegen.c 
mv *.o build 
c++ `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs` \
build/compiler.o build/lexer.o build/ast.o build/parser.o build/list.o build/utils.o build/symtab.o build/codegen.o -o pcc 
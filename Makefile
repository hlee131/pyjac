# build objects with c compiler and link with c++ compiler since llvm is a c++ project with c wrappers 
all:
	make objects && ./build/build.sh pcc

clean:
	rm -f pcc pcc_debug && rm -f build/*.o

objects: 
	gcc `llvm-config --cflags` -c src/compiler.c src/lexer.c src/ast.c src/parser.c src/list.c src/utils.c src/symtab.c src/codegen.c 

debug_objects: 
	gcc `llvm-config --cflags` -c src/compiler.c src/lexer.c src/ast.c src/parser.c src/list.c src/utils.c src/symtab.c src/codegen.c -g 

debug:
	make debug_objects && ./build/build.sh pcc_debug

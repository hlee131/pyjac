# build objects with c compiler and link with c++ compiler since llvm is a c++ project with c wrappers 
all:
	./build/build.sh

clean:
	rm -f a.out debug.out && rm -f build/*.o

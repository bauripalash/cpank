cmake_minimum_required(VERSION 3.0)
project(pankti
	VERSION 0.3.0
	DESCRIPTION "The Pankti Programming Language"
	LANGUAGES C)

add_executable(pankti 
	cpank/api.c 
	cpank/bn.c 
	cpank/builtins.c 
	cpank/compiler.c 
	cpank/debug.c 
	cpank/errmsg.c 
	cpank/htable.c 
	cpank/instruction.c 
	cpank/lexer.c 
	cpank/mem.c 
	cpank/obj.c 
	cpank/openfile.c 
	cpank/runfile.c 
	cpank/utils.c 
	cpank/value.c 
	cpank/vm.c
	cpank/main.c
	cpank/stdlib/array.c 
	cpank/stdlib/big.c 
	cpank/stdlib/bnmath.c 
	cpank/stdlib/bnos.c 
	cpank/stdlib/common.c 
	cpank/stdlib/file.c 
	cpank/stdlib/map.c 
	cpank/stdlib/math.c 
	cpank/stdlib/os.c 
	cpank/stdlib/stdlib.c 
	cpank/stdlib/string.c 
	cpank/ext/baurinum/absneg.c 
	cpank/ext/baurinum/addsub.c 
	cpank/ext/baurinum/baurinum.c 
	cpank/ext/baurinum/compare.c 
	cpank/ext/baurinum/getter.c 
	cpank/ext/baurinum/setter.c 
	cpank/ext/baurinum/utility.c
)

target_link_options(pankti PRIVATE "SHELL:-lm")
# only use cmake, if you really "have to"

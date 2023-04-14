#include "../../cpank/include/api.h"

// #include "../../cpank/include/api.h"
// #include "../../cpank/api.c"
// #include "../../cpank/vm.c"
// #include "../../cpank/obj.c"
// #include "../../cpank/bn.c"
// #include "../../cpank/builtins.c"
// #include "../../cpank/compiler.c"
// #include "../../cpank/debug.c"
// #include "../../cpank/htable.c"
// #include "../../cpank/instruction.c"
// #include "../../cpank/lexer.c"
// #include "../../cpank/mem.c"
// #include "../../cpank/openfile.c"
// #include "../../cpank/runfile.c"
// #include "../../cpank/utils.c"
// #include "../../cpank/value.c"
// #include "../../cpank/stdlib/array.c"
// #include "../../cpank/stdlib/bnmath.c"
// #include "../../cpank/stdlib/common.c"
// #include "../../cpank/stdlib/math.c"
// #include "../../cpank/stdlib/os.c"
// #include "../../cpank/stdlib/stdlib.c"
// #include "../../cpank/stdlib/string.c"
// #include "../../cpank/ext/xxhash/xxhash.c"

char * do_parse_w(char * s){
	char * rs = run_code(s);
	return rs;
}

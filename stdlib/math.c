#include "../include/stdlib.h"
#include "../include/value.h"
#include <math.h>
#include <wchar.h>

Value _math_pow(int argc , Value * args ){
	if (argc != 2) {
		return make_nil();	
	}	
	
	if (args[0].type != args[1].type && args[0].type != V_NUM) {
return make_nil();	
	}

	double a = get_as_number(args[0]);
	double b = get_as_number(args[1]);
	double raw_result = pow(a , b);
	return make_num(raw_result);
	
}


Value _math_add(int argc , Value * args ){
	if (argc != 2) {
		return make_nil();	
	}	
	
	if (args[0].type != args[1].type && args[0].type != V_NUM) {
return make_nil();	
	}

	double a = get_as_number(args[0]);
	double b = get_as_number(args[1]);
	double raw_result = a+b;
	return make_num(raw_result);
	
}



void push_stdlib_math(){
	wchar_t * keys[] = { L"pow" , L"add" };	
	NativeFn fns[] = {_math_pow  , _math_add};
	
	_push_stdlib(L"math", keys, fns, 2);

}

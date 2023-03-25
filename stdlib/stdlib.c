#include "../include/stdlib.h"
#include <stdint.h>
#include <wchar.h>
#include "../include/obj.h"
#include "../include/vm.h"

int _push_stdlib(wchar_t * stdname , wchar_t ** key , NativeFn * fn , int len){

	uint32_t name_hash = get_hash(stdname, wcslen(stdname));
	Module * mod = get_cur_mod();
	for (int i = 0; i < get_cur_mod()->stlib_count; i++) {
		if (mod->stdlibs[i].hash == name_hash) {
			return STDLIB_ALREADY_EXIST_WARNING;	
		}
	}
	
	StdModule * smod = &mod->stdlibs[mod->stlib_count++];
	smod->name = stdname;
	smod->hash = name_hash;
	
	for (int i = 0; i < len; i++) {
		ObjString * k = copy_string(key[i], wcslen(key[i]));
		Value val = make_obj_val(new_native(fn[i]));
		table_set(&smod->items, k, val);
	}
	
	
	return 0;
}

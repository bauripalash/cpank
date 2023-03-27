#include "../include/stdlib.h"

#include <stdint.h>
#include <wchar.h>

#include "../include/obj.h"
#include "../include/utils.h"
#include "../include/vm.h"

int _push_stdlib(wchar_t* stdname, SL funcs[], int len) {
    uint32_t name_hash = get_hash(stdname, wcslen(stdname));
    for (int i = 0; i < vm.stdlib_count; i++) {
        if (vm.stdlibs[i].hash == name_hash) {
            return STDLIB_ALREADY_EXIST_WARNING;
        }
    }

    StdlibMod* smod = &vm.stdlibs[vm.stdlib_count++];
    smod->name = stdname;
    smod->hash = name_hash;
    // smod->owners =
    smod->owner_count = 0;

    for (int i = 0; i < len; i++) {
        SL f = funcs[i];
        ObjString* k = copy_string(f.key, wcslen(f.key));
        cp_color_println('r', L"STD KEY -> %ls", k->chars);
        Value val = make_obj_val(new_native(f.func));
        table_set(&smod->items, k, val);
    }

    return 0;
}

SL msl(wchar_t* key, NativeFn func) { return (SL){.key = key, .func = func}; }

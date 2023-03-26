#include "../include/stdlib.h"

#include <stdint.h>
#include <wchar.h>

#include "../include/obj.h"
#include "../include/vm.h"

int _push_stdlib(wchar_t* stdname, wchar_t** key, NativeFn* fn, int len) {
    uint32_t name_hash = get_hash(stdname, wcslen(stdname));
    for (int i = 0; i < vm.stdlib_count; i++) {
        if (vm.stdlibs[i].hash == name_hash) {
            return STDLIB_ALREADY_EXIST_WARNING;
        }
    }

    StdlibMod* smod = &vm.stdlibs[vm.stdlib_count++];
    smod->name = stdname;
    smod->hash = name_hash;
    // smod->owners = NULL;
    smod->owner_count = 0;

    for (int i = 0; i < len; i++) {
        ObjString* k = copy_string(key[i], wcslen(key[i]));
        Value val = make_obj_val(new_native(fn[i]));
        table_set(&smod->items, k, val);
    }

    return 0;
}

#include "../include/stdlib.h"

#include <stdbool.h>
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
    init_table(&smod->items);
    smod->owner_count = 0;

    for (int i = 0; i < len; i++) {
        SL* f = &funcs[i];
        ObjString* k = copy_string(f->key, wcslen(f->key));
        push(make_obj_val(k));
        ObjNative* nf = new_native(f->func);
        nf->obj.is_gen = true;
        push(make_obj_val(nf));
        table_set(&smod->items, get_as_string(peek_vm(1)), peek_vm(0));
        pop();
        pop();
    }

    // print_table(&smod->items, "AFTER DEFINE OF STDLIB ----------------");

    return 0;
}

SL msl(wchar_t* key, NativeFn func) { return (SL){.key = key, .func = func}; }

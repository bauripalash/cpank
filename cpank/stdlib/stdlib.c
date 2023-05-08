/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "../include/stdlib.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <uchar.h>
#include <wchar.h>

#include "../include/obj.h"
#include "../include/utils.h"
#include "../include/vm.h"

int _push_stdlib(PankVm* vm, char32_t* stdname, SL funcs[], int len) {
    uint32_t name_hash = get_hash(stdname, strlen32(stdname));
    for (int i = 0; i < vm->stdlib_count; i++) {
        if (vm->stdlibs[i].hash == name_hash) {
            return STDLIB_ALREADY_EXIST_WARNING;
        }
    }

    StdlibMod* smod = &vm->stdlibs[vm->stdlib_count++];
    smod->name = stdname;
    smod->hash = name_hash;
    init_table(&smod->items);
    smod->owner_count = 0;

    for (int i = 0; i < len; i++) {
        SL* f = &funcs[i];
        //cp_println(L"->f.key->%ls" , f->key);
        ObjString* k = copy_string(vm, f->key, strlen32(f->key));
        push(vm, make_obj_val(k));
        ObjNative* nf = new_native(vm, f->func, f->key);
        nf->obj.is_gen = true;
        push(vm, make_obj_val(nf));
        table_set(vm, &smod->items, get_as_string(peek_vm(vm, 1)),
                  peek_vm(vm, 0));
        pop(vm);
        pop(vm);
    }

    // print_table(&smod->items, "AFTER DEFINE OF STDLIB ----------------");

    return 0;
}

SL msl(char32_t* key, NativeFn func) { return (SL){.key = key, .func = func}; }

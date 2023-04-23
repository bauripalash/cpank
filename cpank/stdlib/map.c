#include <stdbool.h>

#include "../include/stdlib.h"
#include "../include/vm.h"

Value _map_exists(PankVm* vm, int argc, Value* args) {
    check_argc_count("iskey(map , key)", 2, argc);
    Value raw_map = args[0];

    if (!is_map_obj(raw_map)) {
        return make_error(
            vm, U"first argument to iskey(map, key) must be a hashmap");
    }

    Value key = args[1];
    ObjHashMap* hmap = get_as_hmap(raw_map);
    for (int i = 0; i < hmap->cap; i++) {
        HmapItem k = hmap->items[i];
        if (!is_nil(k.key) && is_equal(k.key, key)) {
            return make_bool(true);
        }
    }
    return make_bool(false);
}

Value _map_get_keys(PankVm* vm, int argc, Value* args) {
    check_argc_count("keys(map)", 1, argc);
    Value raw_map = args[0];
    if (!is_map_obj(raw_map)) {
        return make_error(vm, U"argument to keys(map) must be a hashmap");
    }

    ObjHashMap* hmap = get_as_hmap(raw_map);
    ObjArray* karr = new_array(vm);
    push(vm, make_obj_val(karr));

    for (int i = 0; i < hmap->cap; i++) {
        HmapItem k = hmap->items[i];
        if (!is_nil(k.key)) {
            write_valarr(vm, &karr->items, k.key);
        }
    }

    karr->len = karr->items.len;

    return pop(vm);
}

Value _map_get_values(PankVm* vm, int argc, Value* args) {
    check_argc_count("values(map)", 1, argc);
    Value raw_map = args[0];
    if (!is_map_obj(raw_map)) {
        return make_error(vm, U"argument to values(map) must be a hashmap");
    }

    ObjHashMap* hmap = get_as_hmap(raw_map);
    ObjArray* varr = new_array(vm);
    push(vm, make_obj_val(varr));

    for (int i = 0; i < hmap->cap; i++) {
        HmapItem k = hmap->items[i];
        if (!is_nil(k.key)) {
            write_valarr(vm, &varr->items, k.val);
        }
    }

    varr->len = varr->items.len;

    return pop(vm);
}

void push_stdlib_map(PankVm* vm) {
    SL sls[] = {
        msl(U"iskey", _map_exists),
        msl(U"keys", _map_get_keys),
        msl(U"values", _map_get_values),
    };

    _push_stdlib(vm, U"map", sls, 3);
}

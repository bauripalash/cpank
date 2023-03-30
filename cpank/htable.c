#include "include/htable.h"

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include "include/mem.h"
#include "include/obj.h"
#include "include/value.h"

#define TABLE_MAX_LD 0.75

void debug_entry(Entry *entry) {
    wprintf(L"ENTRY[ K(%ls); V(", entry->key->chars);
    print_val(entry->val);
    wprintf(L"); H(%u)\n", entry->key->hash);
}

void init_table(Htable *table) {
    table->len = 0;
    table->cap = 0;
    table->entries = NULL;
}

void free_table(PankVm *vm, Htable *table) {
    FREE_ARR(vm, Entry, table->entries, table->cap);
    init_table(table);
}

static Entry *find_entry(Entry *entries, int cap, ObjString *key) {
    uint32_t index = key->hash & (cap - 1);
    Entry *tombstone = NULL;
    for (;;) {
        Entry *entry = &entries[index];
        if (entry->key == NULL) {
            if (is_nil(entry->val)) {
                return tombstone != NULL ? tombstone : entry;
            } else {
                if (tombstone == NULL) {
                    tombstone = entry;
                }
            }
        } else if (entry->key->len == key->len) {
            if (wcscmp(entry->key->chars, key->chars) == 0) {
                return entry;
            }
        }

        index = (index + 1) & (cap - 1);
    }
}

static void adjust_cap(PankVm *vm, Htable *table, int cap) {
    // wprintf(L"ADJUST CAP -> len->%d | cap->%d | nowcap->%d\n" , table->len ,
    // table->cap , cap);

    // wprintf(L"loop -> table cap %d\n" , table->cap);
    Entry *entries = ALLOC(vm, Entry, cap);
    for (int i = 0; i < cap; i++) {
        entries[i].key = NULL;
        entries[i].val = make_nil();

        // wprintf(L"loop -> table cap %d\n" , table->cap);
    }

    // wprintf(L"loop -> table cap %d\n" , table->cap);
    table->len = 0;

    // wprintf(L"loop -> table cap %d\n" , table->cap);
    for (int i = 0; i < table->cap; i++) {
        // wprintf(L"loop -> table cap %d\n" , table->cap);

        // wprintf(L"LOOP_> ADJUST CAP -> len->%d | cap->%d | nowcap->%d\n" ,
        // table->len , table->cap , cap);
        Entry *entry = &table->entries[i];
        // if (entry == NULL) {
        // continue;
        // }
        // wprintf(L"ADJUST CAP -> %ls \n" , entry->key->chars);
        if (entry->key == NULL) {
            continue;
        }

        Entry *dest = find_entry(entries, cap, entry->key);
        dest->key = entry->key;
        dest->val = entry->val;
        table->len++;
    }
    FREE_ARR(vm, Entry, table->entries, table->cap);

    table->entries = entries;
    table->cap = cap;

    // wprintf(L"END CAP -> len->%d | cap->%d | nowcap->%d\n" , table->len ,
    // table->cap , cap);
}

bool table_set(PankVm *vm, Htable *table, ObjString *key, Value value) {
    if (table->len + 1 > table->cap * TABLE_MAX_LD) {
        int cap = GROW_CAP(table->cap);
        // wprintf(L"growing table for -> %ls\n" , key->chars);
        adjust_cap(vm, table, cap);
    }

    Entry *entry = find_entry(table->entries, table->cap, key);
    bool is_new_key = entry->key == NULL;

    if (is_new_key && is_nil(entry->val)) {
        table->len++;
    }

    entry->key = key;
    // wmemcpy(entry->key->chars, key->chars, key->len);
    entry->val = value;
    // wprintf(L"setting to table KEY->'%ls'\n VALUE -> " , entry->key->chars);
    // print_val(value);
    // wprintf(L"\n");
    return is_new_key;
}

bool table_get(Htable *table, ObjString *key, Value *value) {
    if (table->len == 0) {
        return false;
    }

    Entry *entry = find_entry(table->entries, table->cap, key);
    if (entry->key == NULL) {
        return false;
    }

    *value = entry->val;

    // debug_entry(entry);
    return true;
}

bool table_del(Htable *table, ObjString *key) {
    if (table->len == 0) {
        return false;
    }

    Entry *entry = find_entry(table->entries, table->cap, key);
    if (entry->key == NULL) {
        return false;
    }

    entry->key = NULL;
    entry->val = make_bool(true);
    return true;
}

void copy_table(PankVm *vm, Htable *from, Htable *to) {
    for (int i = 0; i < from->cap; i++) {
        Entry *entry = &from->entries[i];
        if (entry->key != NULL) {
            table_set(vm, to, entry->key, entry->val);
        }
    }
}

void print_table(Htable *table, char *name) {
    wprintf(L"<- TABLE (%s)->\n", name);
    for (int i = 0; i < table->cap; i++) {
        Entry *entry = &table->entries[i];
        if (entry != NULL && entry->key != NULL) {
            wprintf(L"[ key -> '%ls' | val -> ", entry->key->chars);
            print_val(entry->val);
            wprintf(L" ]\n");
        }
    }

    wprintf(L"<- END TABLE ->\n");
}

ObjString *table_find_str(Htable *table, wchar_t *chars, int len,
                          uint32_t hash) {
    if (table->len == 0) {
        return NULL;
    }

    uint32_t index = hash & (table->cap - 1);

    for (;;) {
        Entry *entry = &table->entries[index];
        if (entry->key == NULL) {
            if (is_nil(entry->val)) {
                return NULL;
            }
        } else if (entry->key->len == len && entry->key->hash == hash &&
                   wcscmp(entry->key->chars, chars) == 0) {
            return entry->key;
        }

        index = (index + 1) & (table->cap - 1);
    }
}

void mark_table(PankVm *vm, Htable *table) {
    for (int i = 0; i < table->cap; i++) {
        Entry *entry = &table->entries[i];
        mark_obj(vm, (Obj *)entry->key);
        mark_val(vm, entry->val);
    }
}

void table_remove_white(Htable *table) {
    for (int i = 0; i < table->cap; i++) {
        Entry *entry = &table->entries[i];
        if (entry->key != NULL && !entry->key->obj.is_marked) {
            table_del(table, entry->key);
        }
    }
}

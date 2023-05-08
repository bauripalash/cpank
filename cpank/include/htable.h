/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_htable_h
#define cpank_htable_h
#include <stdbool.h>
#include <stdint.h>
#include <uchar.h>

#include "pank.h"
#include "value.h"
typedef struct {
    ObjString *key;  // TO DO : use hashed key instead of strings;
    Value val;
} Entry;

typedef struct {
    int len;
    int cap;
    Entry *entries;
} Htable;

void debug_entry(Entry *entry);
void init_table(Htable *table);
void free_table(PankVm *vm, Htable *table);
void copy_table(PankVm *vm, Htable *from, Htable *to);
bool table_set(PankVm *vm, Htable *table, ObjString *key, Value value);
bool table_get(Htable *table, ObjString *key, Value *value);
bool table_del(Htable *table, ObjString *key);
ObjString *table_find_str(Htable *table, char32_t *chars, int len,
                          uint64_t hash);

void print_table(Htable *table, char *name);
void mark_table(PankVm *vm, Htable *table);
void table_remove_white(Htable *table);

#endif

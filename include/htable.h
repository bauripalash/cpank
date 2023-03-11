#include "value.h"
#include <stdbool.h>
#include <stdint.h>
#ifndef cpank_htable_h
#define cpank_htable_h

typedef struct {
  ObjString *key; // TO DO : use hashed key instead of strings;
  Value val;
} Entry;

typedef struct {
  int len;
  int cap;
  Entry *entries;
} Htable;

void init_table(Htable *table);
void free_table(Htable *table);
void copy_table(Htable *from, Htable *to);
bool table_set(Htable *table, ObjString *key, Value value);
bool table_get(Htable *table, ObjString *key, Value *value);
bool table_del(Htable *table, ObjString *key);
ObjString *table_find_str(Htable *table, wchar_t *chars, int len,
                          uint32_t hash);

#endif

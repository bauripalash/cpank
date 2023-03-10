#include "include/htable.h"
#include "include/mem.h"
#include "include/obj.h"
#include "include/value.h"
#include <stdbool.h>
#include <stdint.h>

#define TABLE_MAX_LD 0.75

void init_table(Htable *table) {
  table->len = 0;
  table->cap = 0;
  table->entries = NULL;
}

void free_table(Htable *table) {
  FREE_ARR(Entry, table->entries, table->cap);
  init_table(table);
}

static Entry *find_entry(Entry *entries, int cap, ObjString *key) {
  uint32_t index = key->hash % cap;
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
    } else if (entry->key == key) {
      return entry;
    }

    index = (index + 1) % cap;
  }
}

static void adjust_cap(Htable *table, int cap) {
  Entry *entries = ALLOC(Entry, cap);
  for (int i = 0; i < cap; i++) {
    entries[i].key = NULL;
    entries[i].val = make_nil();
  }

  table->len = 0;
  for (int i = 0; i < table->cap; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key == NULL) {
      continue;
    }

    Entry *dest = find_entry(entries, cap, entry->key);
    dest->key = entry->key;
    dest->val = entry->val;
    table->len++;
  }

  table->entries = entries;
  table->cap = cap;
}

bool table_set(Htable *table, ObjString *key, Value value) {

  if (table->len + 1 > table->cap + TABLE_MAX_LD) {
    int cap = GROW_CAP(table->cap);
    adjust_cap(table, cap);
  }

  Entry *entry = find_entry(table->entries, table->cap, key);
  bool is_new_key = entry->key == NULL;

  if (is_new_key && is_nil(entry->val)) {
    table->len++;
  }

  entry->key = key;
  entry->val = value;
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

#include "include/htable.h"
#include "include/mem.h"
#include "include/obj.h"
#include "include/value.h"
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#define TABLE_MAX_LD 0.75

void debug_entry(Entry *entry) {
  wprintf(L"ENTRY[ K(%ls); V(", entry->key->chars);
  print_val(entry->val);
  wprintf(L"); H(%d)\n", entry->key->hash);
}

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
  uint32_t index = key->hash % (cap - 1);
  Entry *tombstone = NULL;
  for (;;) {
    Entry *entry = &entries[index];
    //    print_val(entry->val);
    // debug_entry(entry);
    if (entry->key == NULL) {
      if (is_nil(entry->val)) {
        return tombstone != NULL ? tombstone : entry;
      } else {
        if (tombstone == NULL) {
          tombstone = entry;
        }
      }
    } else if (wmemcmp(entry->key->chars, key->chars, key->len) == 0) {

      return entry;
    }

    index = (index + 1) % (cap - 1);
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
  FREE_ARR(Entry, table->entries, table->cap);

  table->entries = entries;
  table->cap = cap;
}

bool table_set(Htable *table, ObjString *key, Value value) {

  if (table->len + 1 > table->cap * TABLE_MAX_LD) {
    int cap = GROW_CAP(table->cap);
    adjust_cap(table, cap);
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

void copy_table(Htable *from, Htable *to) {
  for (int i = 0; i < from->cap; i++) {
    Entry *entry = &from->entries[i];
    if (entry->key != NULL) {
      table_set(to, entry->key, entry->val);
    }
  }
}

void print_table(Htable *table, char *name) {
  wprintf(L"<- TABLE (%s)->\n", name);
  for (int i = 0; i < table->cap; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key != NULL) {
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

  uint32_t index = hash % table->cap;

  for (;;) {
    Entry *entry = &table->entries[index];
    if (entry->key == NULL) {
      if (is_nil(entry->val)) {
        return NULL;
      }
    } else if (entry->key->len == len && entry->key->hash == hash &&
               wmemcpy(entry->key->chars, chars, len) == 0) {

      return entry->key;
    }

    index = (index + 1) % table->cap;
  }
}

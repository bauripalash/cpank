#ifndef cpank_obj_h
#define cpank_obj_h

#include <stdbool.h>
#include <stdint.h>
#include <uchar.h>
#include <wchar.h>

#include "common.h"
#include "htable.h"
#include "instruction.h"
#include "value.h"
// #include "pank.h"

typedef enum {
    OBJ_STR = 0,
    OBJ_FUNC,
    OBJ_NATIVE,
    OBJ_CLOUSRE,
    OBJ_UPVAL,
    OBJ_MOD,
    OBJ_ARRAY,
    OBJ_ERR,
    OBJ_HMAP,
} ObjType;

struct Obj {
    ObjType type;
    bool is_marked;
    bool is_gen;
    struct Obj *next;
};

typedef struct {
    Obj obj;
    int arity;
    Instruction ins;
    ObjString *name;
    int up_count;
} ObjFunc;

typedef struct ObjUpVal {
    Obj obj;
    Value *location;
    struct ObjUpVal *next;
    Value closed;
} ObjUpVal;

typedef Value (*NativeFn)(PankVm *vm, int argc, Value *args);

typedef struct {
    Obj obj;
    NativeFn func;
    char32_t *name;
    int name_len;
} ObjNative;

ObjNative *new_native(PankVm *vm, NativeFn fn, char32_t *name);

typedef struct {
    Obj obj;
    ObjFunc *func;
    ObjUpVal **upv;
    int upv_count;
    uint32_t global_owner;
    Htable *globals;
} ObjClosure;
ObjClosure *new_closure(PankVm *vm, ObjFunc *function, uint32_t global_owner);

struct ObjString {
    Obj obj;
    int len;
    char32_t *chars;
    uint32_t hash;
};

typedef struct {
    Obj obj;
    int len;
    char *errmsg;
} ObjErr;

typedef struct ObjArray {
    Obj obj;
    int len;
    Valarr items;
} ObjArray;

typedef struct {
    Value key;
    Value val;
    uint32_t hash;
} HmapItem;

typedef struct {
    Obj obj;
    int count;
    int cap;
    HmapItem *items;
} ObjHashMap;

ObjHashMap *new_hmap(PankVm *vm);
ObjFunc *new_func(PankVm *vm);

ObjUpVal *new_up_val(PankVm *vm, Value *val);
ObjArray *new_array(PankVm *vm);
ObjType get_obj_type(Value val);
bool is_obj_type(Value val, ObjType ot);
bool is_str_obj(Value val);
bool is_func_obj(Value val);
bool is_native_obj(Value val);
bool is_closure_obj(Value val);
bool is_mod_obj(Value val);
bool is_err_obj(Value val);
bool is_array_obj(Value val);
ObjErr *new_err_obj(PankVm *vm, char32_t *errmsg);
Value make_error(PankVm *vm, char32_t *errmsg);
ObjErr *get_as_err(Value val);
ObjFunc *get_as_func(Value val);
ObjClosure *get_as_closure(Value val);
ObjString *get_as_string(Value val);
ObjArray *get_as_array(Value val);
char32_t *get_as_native_string(Value val);
NativeFn get_as_native(Value val);
ObjString *copy_string(PankVm *vm, char32_t *chars, int len);
ObjString *take_string(PankVm *vm, char32_t *chars, int len);
void print_obj(Value val);
wchar_t *get_obj_type_as_string(ObjType o);
uint32_t get_hash(const char32_t *key, int len);

bool hmap_get(ObjHashMap *map, Value key, Value *val);

bool hmap_set(PankVm *vm, ObjHashMap *map, Value key, Value val);

ObjHashMap *get_as_hmap(Value val);

bool is_map_obj(Value val);

bool is_obj_equal(Obj *a, Obj *b);

bool is_valid_hashmap_key(Value val);
/*
#define get_as_native(value) \
  (((ObjNative *)get_as_obj(value))->func)
*/
#endif

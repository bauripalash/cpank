/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_obj_h
#define cpank_obj_h
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>
#include <uchar.h>
#include <wchar.h>

#include "../ext/baurinum/baurinum.h"
#include "common.h"
#include "htable.h"
#include "instruction.h"
#include "value.h"
// #include "pank.h"

#define BIGFLOAT_MINPREC 200
#define BIGFLOAT_ROUND   MPFR_RNDU

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
    OBJ_BIGNUM,
} ObjType;

struct Obj {
    ObjType type;
    bool is_marked;
    bool is_gen;
    bool is_virt;
    int tok_colpos;
    int tok_len;
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
    uint64_t hash;
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
    uint64_t hash;
} HmapItem;

typedef struct {
    Obj obj;
    int count;
    int cap;
    HmapItem *items;
} ObjHashMap;

typedef struct {
    Obj obj;
    bool marker;
    bool isfloat;
    union {
        bnum ival;
        long double fval;
    } as;
} ObjBigNum;

ObjBigNum *new_bignum(PankVm *vm);
ObjBigNum *new_bignum_float(PankVm *vm);
// ObjBigNum *new_bignum_with_mpf(PankVm *vm, mpfr_t value);
// ObjBigNum *new_bignum_with_mpz(PankVm *vm, mpz_t value);
//

ObjBigNum *new_bignum_with_mpint(PankVm *vm, bnum *ival);
ObjBigNum *new_bignum_with_double(PankVm *vm, double value);
ObjBigNum *new_bignum_with_ld(PankVm *vm, long double value);
ObjBigNum *new_bignum_with_str(PankVm *vm, char32_t *value);
ObjBigNum *get_as_bignum(Value value);
bool is_bignum_obj(Value value);

ObjHashMap *new_hmap(PankVm *vm);
ObjFunc *new_func(PankVm *vm);

ObjUpVal *new_up_val(PankVm *vm, Value *val);
ObjArray *new_array(PankVm *vm);
ObjType get_obj_type(Value val);
char32_t *get_obj_type_str(Value val, bool isbn);
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
uint64_t get_hash(const char32_t *key, int len);

bool hmap_get(ObjHashMap *map, Value key, Value *val);

bool hmap_set(PankVm *vm, ObjHashMap *map, Value key, Value val);

ObjHashMap *get_as_hmap(Value val);

bool is_map_obj(Value val);

bool is_obj_equal(Obj *a, Obj *b);

bool is_valid_hashmap_key(Value val);

Value make_str(PankVm *vm, char32_t *str);

Value make_argc_want_mismatch_error(PankVm *vm, char *funname, int want,
                                    int got);

Value make_bn_argc_want_mismatch_error(PankVm *vm, char *funname, int want,
                                       int got);
// There 'must' be some bugs or edge cases, testing is required
char32_t *obj_to_string(PankVm *vm, Value val);

#define ALLOCATE_OBJ(vm, type, otype) (type *)alloc_obj(vm, sizeof(type), otype)
/*
#define get_as_native(value) \
  (((ObjNative *)get_as_obj(value))->func)
*/

#ifdef __cplusplus
}
#endif
#endif

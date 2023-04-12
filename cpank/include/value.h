#ifndef cpank_value_h
#define cpank_value_h
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <uchar.h>

#include "common.h"
#include "pank.h"

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum { V_BOOL, V_NIL, V_NUM, V_OBJ } ValType;

#ifdef NAN_BOXING
typedef uint64_t Value;
 #define QNAN      ((uint64_t)0x7ffc000000000000)
 #define SIGN_BIT  ((uint64_t)0x8000000000000000)
 #define TAG_NIL   1
 #define TAG_FALSE 2
 #define TAG_TRUE  3
 #define NIL_VAL   ((Value)(uint64_t)(QNAN | TAG_NIL))
 #define FALSE_VAL ((Value)(uint64_t)(QNAN | TAG_FALSE))
 #define TRUE_VAL  ((Value)(uint64_t)(QNAN | TAG_TRUE))

static inline double value_to_number(Value val) {
    double num;
    memcpy(&num, &val, sizeof(Value));
    return num;
}
static inline Value num_to_value(double num) {
    Value value;
    memcpy(&value, &num, sizeof(double));
    return value;
}

 #define is_bool(value)       (((value) | 1) == TRUE_VAL)
 #define is_nil(value)        ((value) == NIL_VAL)
 #define is_num(value)        (((value)&QNAN) != QNAN)
 #define is_obj(value)        (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

 #define get_as_bool(value)   ((value) == TRUE_VAL)
 #define get_as_obj(value)    ((Obj *)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))
 #define get_as_number(value) value_to_number(value)

 #define make_num(num)        num_to_value(num)
 #define make_obj_val(obj)    (Value)(SIGN_BIT | QNAN | (uint64_t)(uintptr_t)(obj))
 #define make_bool(b)         ((b) ? TRUE_VAL : FALSE_VAL)
 #define make_nil             ((Value)(uint64_t)(QNAN | TAG_NIL))
 #define make_neg(value)      (num_to_value(-value_to_number(value)))

#else

typedef struct {
    ValType type;
    union {
        bool boolean;
        double num;
        Obj *obj;
    } as;
} Value;

static inline double value_to_number(Value val) { return val.as.num; }

static inline Value num_to_value(double num) {
    return (Value){.as.num = num, .type = V_NUM};
}

 #define is_bool(value)       ((value).type == V_BOOL)
 #define is_nil(value)        ((value).type == V_NIL)
 #define is_num(value)        ((value).type == V_NUM)
 #define is_obj(value)        ((value).type == V_OBJ)

 #define get_as_bool(value)   ((value).as.boolean)
 #define get_as_number(value) ((value).as.num)
 #define get_as_obj(value)    ((value).as.obj)

 #define make_bool(bval)      ((Value){V_BOOL, {.boolean = bval}})
 #define make_nil             ((Value){V_NIL, {.num = 0}})
 #define make_num(value)      ((Value){V_NUM, {.num = value}})
 #define make_obj_val(object) ((Value){V_OBJ, {.obj = (Obj *)object}})
 #define make_neg(value)      (num_to_value(-value_to_number(value)))

void print_val_type(ValType vt);

#endif

bool is_falsey(Value val);
bool is_equal(Value left, Value right);
char32_t *get_val_type_str(Value val, bool isbn);
char32_t *value_to_string(PankVm *vm, Value val);

typedef struct {
    int cap;
    int len;
    Value *values;
} Valarr;

void init_valarr(Valarr *array);
void write_valarr(PankVm *vm, Valarr *array, Value value);
void free_valarr(PankVm *vm, Valarr *array);
void pop_valarr(PankVm *vm, Valarr *array);
void print_val(Value val);

#endif

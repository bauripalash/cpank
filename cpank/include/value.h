#ifndef cpank_value_h
#define cpank_value_h
#include "common.h"

typedef struct Obj Obj;
typedef struct ObjString ObjString;
typedef enum { V_BOOL, V_NIL, V_NUM, V_OBJ } ValType;

void print_val_type(ValType vt);

// typedef double Value;
//

typedef struct {
    ValType type;
    union {
        bool boolean;
        double num;
        Obj *obj;
    } as;
} Value;

#define get_as_obj(value) ((value).as.obj)
#define make_obj_val(object) ((Value){V_OBJ, {.obj = (Obj *)object}})
bool is_bool(Value val);
bool is_nil(Value val);
bool is_num(Value val);
bool is_falsey(Value val);
bool is_equal(Value left, Value right);
bool is_obj(Value val);

bool get_as_bool(Value val);
double get_as_number(Value val);

Value make_bool(bool value);
Value make_nil(void);
Value make_num(double num);
Value make_neg(Value value);

typedef struct {
    int cap;
    int len;
    Value *values;
} Valarr;

void init_valarr(Valarr *array);
void write_valarr(Valarr *array, Value value);
void free_valarr(Valarr *array);
void print_val(Value val);

#endif

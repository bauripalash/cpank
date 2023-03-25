#ifndef cpank_vm_h
#define cpank_vm_h

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include "common.h"
#include "htable.h"
#include "instruction.h"
#include "obj.h"
#include "value.h"

#define FRAME_SIZE 64
#define MODULES_MAX 10
#define STACK_SIZE (FRAME_SIZE * UINT8_COUNT)

typedef struct {
    ObjClosure *closure;
    uint8_t *ip;
    Value *slots;
    uint32_t global_owner;
    Htable *globals;
} CallFrame;

typedef struct Module {
    Htable globals;
    CallFrame frames[FRAME_SIZE];
    int frame_count;
    wchar_t *name;
    ObjUpVal *open_upvs;
    bool is_default;
    struct Module *origin;

} Module;

typedef struct ObjMod {
    Obj obj;
    ObjString *name;
} ObjMod;
Module *get_mod_by_hash(uint32_t hash);
ObjMod *get_as_mod(Value val);     // defined in obj.c
ObjMod *new_mod(ObjString *name);  // define in obj.c

void init_module(Module *mod, const wchar_t *name);
Module *get_cur_mod();
bool is_default(Module *mod);

typedef struct {
    // Instruction *ins;
    // uint8_t *ip;
    Value stack[STACK_SIZE];
    Value *stack_top;
    Htable strings;
    Module modules[MODULES_MAX];
    uint32_t mod_names[MODULES_MAX];
    int mod_count;
    Module *current_mod;
    // Htable globals;
    Obj *objs;
    // CallFrame frames[FRAME_SIZE];
    // int frame_count;
    // ObjUpVal *open_upvs;
    int gray_count;
    int gray_cap;
    Obj **gray_stack;
    size_t bts_allocated;
    size_t next_gc;
    Value last_pop;
} Vm;

typedef enum {
    INTRP_OK,
    INTRP_COMPILE_ERR,
    INTRP_RUNTIME_ERR,
} IResult;

extern Vm vm;

void boot_vm();
void free_vm();
IResult interpret(wchar_t *source);
void push(Value value);
Value pop();
Value get_last_pop();
bool call_val(Value calle, int argc);
bool call(ObjClosure *closure, int origin, int argc);
void define_native(wchar_t *name, NativeFn func);
Value clock_ntv_fn(int argc, Value *args);
ObjUpVal *capture_upv(Module *module, Value *local);
void close_upval(Module *module, Value *last);

#endif

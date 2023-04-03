#ifndef cpank_vm_h
#define cpank_vm_h

#include <stdbool.h>
#include <stdint.h>
#include <uchar.h>
#include <wchar.h>

#include "common.h"
#include "compiler.h"
#include "htable.h"
#include "instruction.h"
#include "obj.h"
#include "value.h"

#define FRAME_SIZE 64
#define MODULES_MAX 64
#define STDLIB_MAX 64
#define STACK_SIZE (FRAME_SIZE * UINT8_COUNT)

typedef struct {
    ObjClosure *closure;
    uint8_t *ip;
    Value *slots;
    uint32_t global_owner;
    Htable *globals;
} CallFrame;

typedef struct StdlibMod {
    Htable items;
    char16_t *name;
    uint32_t hash;
    uint32_t owners[MODULES_MAX];
    int owner_count;
} StdlibMod;

typedef struct StdProxy {
    StdlibMod *stdmod;
    char16_t *origin_name;
    char16_t *proxy_name;
    uint32_t proxy_hash;
} StdProxy;

typedef struct Module {
    Htable globals;
    StdProxy stdproxy[STDLIB_MAX];
    int stdlib_count;
    CallFrame frames[FRAME_SIZE];
    int frame_count;
    wchar_t *name;
    uint32_t hash;
    ObjUpVal *open_upvs;
    bool is_default;
    struct Module *origin;
    char16_t *source_code;

} Module;

typedef struct ObjMod {
    Obj obj;
    ObjString *name;

} ObjMod;
Module *get_mod_by_hash(PankVm *vm, uint32_t hash);
ObjMod *get_as_mod(Value val);                // defined in obj.c
ObjMod *new_mod(PankVm *vm, char16_t *name);  // define in obj.c

void init_module(Module *mod, const char16_t *name);
Module *get_cur_mod(PankVm *vm);
bool is_default(Module *mod);

// The Vm struct
struct _Vm {
    Compiler *compiler;
    // The stack with max size of STACK_SIZE
    Value stack[STACK_SIZE];
    // Top of the stack
    Value *stack_top;
    // All allocated strings
    Htable strings;
    // List of modules imported and a default one
    Module modules[MODULES_MAX];
    // Builtin functions
    Htable builtins;
    // Standard library
    StdlibMod stdlibs[STDLIB_MAX];
    // How many Standard library packages imported by all modules
    // See more on `notes`
    int stdlib_count;
    // Hash of all modules
    // Hash is used so that we don't have to compare strings everytime
    uint32_t mod_names[MODULES_MAX];
    // Number of modules imported
    int mod_count;
    // The current mod being run
    Module *current_mod;
    // All allocated objects
    Obj *objs;
    int gray_count;
    int gray_cap;
    Obj **gray_stack;
    size_t bts_allocated;
    size_t next_gc;
    Value last_pop;
};

typedef enum {
    INTRP_OK,
    INTRP_COMPILE_ERR,
    INTRP_RUNTIME_ERR,
} IResult;

// extern Vm vm;

// Initilalize the VM
PankVm *boot_vm(void);

// Free the VM and all things it holds
void free_vm(PankVm *vm);
IResult interpret(PankVm *vm, char16_t *source);
bool push(PankVm *vm, Value value);
Value pop(PankVm *vm);
Value peek_vm(PankVm *vm, int dist);
Value get_last_pop(PankVm *vm);

void define_native(PankVm *vm, char16_t *name, NativeFn func);
Value clock_ntv_fn(PankVm *vm, int argc, Value *args);

#endif

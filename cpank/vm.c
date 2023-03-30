#include "include/vm.h"

#include <locale.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#include "include/common.h"
#include "include/compiler.h"
#include "include/debug.h"
#include "include/instruction.h"
#include "include/mem.h"
#include "include/obj.h"
#include "include/openfile.h"
#include "include/stdlib.h"
#include "include/utils.h"
#include "include/value.h"

// PankVm vm;
const wchar_t *default_mod = L"_d_";
// #define DEBUG_STACK

void reset_stack(PankVm *vm) { vm->stack_top = vm->stack; }

PankVm *boot_vm(void) {
    PankVm *vm = malloc(sizeof(PankVm));
    reset_stack(vm);
    gcon.is_paused = false;
    vm->objs = NULL;
    vm->last_pop = make_nil();

    vm->bts_allocated = 0;
    vm->next_gc = 1024 * 1024;
    vm->gray_cap = 0;
    vm->gray_count = 0;
    vm->gray_stack = NULL;
    vm->mod_count = 0;

    init_table(&vm->strings);
    init_table(&vm->builtins);

    vm->mod_names[vm->mod_count] = 0;
    Module *dmod = &vm->modules[vm->mod_count++];
    init_module(dmod, default_mod);
    dmod->is_default = true;
    dmod->origin = NULL;
    vm->mod_names[vm->mod_count] = get_hash(default_mod, wcslen(default_mod));
    vm->current_mod = dmod;
    define_native(vm, L"clock", clock_ntv_fn);
    return vm;
}

void free_stdlibs(PankVm *vm) {
    for (int i = 0; i < vm->stdlib_count; i++) {
        StdlibMod *mod = &vm->stdlibs[i];
        if (mod->items.len > 0 || mod->items.cap > 0) {
            free_table(vm, &mod->items);
        }
    }
}

void init_module(Module *mod, const wchar_t *name) {
    init_table(&mod->globals);
    // init_stdlib_module();
    mod->frame_count = 0;
    mod->stdlib_count = 0;
    mod->hash = get_hash(name, wcslen(name));
    mod->name = malloc(sizeof(wchar_t) * (wcslen(name) + 1));
    wmemcpy(mod->name, name, wcslen(name) + 1);
    mod->open_upvs = NULL;
    mod->source_code = NULL;
}

bool is_default(Module *mod) {
    return wcscmp(mod->name, default_mod) == 0 && mod->is_default;
}

Module *get_mod_by_hash(PankVm *vm, uint32_t hash) {
    for (int i = 0; i < vm->mod_count; i++) {
        if (vm->mod_names[i] == hash) {
            return &vm->modules[i];
        }
    }
    return NULL;
}

StdlibMod *get_stdlib_by_hash(PankVm *vm, uint32_t hash, Module *curmod) {
    // uint32_t curmod_hash = get_hash(curmod->name, wcslen(curmod->name));
    uint32_t curmod_hash = curmod->hash;
    for (int i = 0; i < vm->stdlib_count; i++) {
        StdlibMod *m = &vm->stdlibs[i];
        if (m->hash == hash) {
            // cp_color_println('y', L"stm -> %ld | %d" , m->hash ,
            // m->owner_count );
            for (int j = 0; j < m->owner_count; j++) {
                if (m->owners[j] == curmod_hash) {
                    return m;
                }
            }
        }
    }
    return NULL;
}

uint32_t get_proxy_hash(uint32_t name, Module *curmod) {
    for (int i = 0; i < curmod->stdlib_count; i++) {
        StdProxy *proxy = &curmod->stdproxy[i];
        if (proxy->proxy_hash == name) {
            return proxy->stdmod->hash;
        }
    }

    return 0;
}

void print_mod_names(PankVm *vm) {
    for (int i = 0; i < vm->mod_count; i++) {
        wprintf(L"M| %4d -> %ld -> %ls \n", i, vm->mod_names[i],
                vm->modules[i].name);
    }
}
void free_module(PankVm *vm, Module *mod) {
    free_table(vm, &mod->globals);

    mod->frame_count = 0;
    if (mod->source_code != NULL) {
        free(mod->source_code);
    }

    free(mod->name);
}

void define_native(PankVm *vm, wchar_t *name, NativeFn func) {
    push(vm, make_obj_val(copy_string(vm, name, (int)wcslen(name))));  // peek 1
    push(vm, make_obj_val(new_native(vm, func)));                      // peek 0
    table_set(vm, &vm->builtins, get_as_string(vm->stack[0]), vm->stack[1]);
    pop(vm);
    pop(vm);
}

void print_modframes(PankVm *vm) {
    for (int i = 0; i < vm->mod_count; i++) {
        Module *mod = &vm->modules[i];
        wprintf(L"module %ls has %d frames \n", mod->name, mod->frame_count);
        for (int f = 0; f < mod->frame_count; f++) {
            CallFrame *fr = &mod->frames[f];
            wchar_t *name = L"unknown";
            if (fr->closure != NULL) {
                if (fr->closure->func != NULL) {
                    if (fr->closure->func->name != NULL) {
                        if (fr->closure->func->name->chars != NULL) {
                            name = fr->closure->func->name->chars;
                        }
                    }
                }
            }
            wprintf(L"]] FRAME %d | %ls\n", f, name);
        }
    }
}

void free_vm(PankVm *vm) {
    free_table(vm, &vm->strings);
    free_table(vm, &vm->builtins);
    free_stdlibs(vm);
    for (int i = 0; i < vm->mod_count; i++) {
        free_module(vm, &vm->modules[i]);
    }
    free_objs(vm);
}

Value get_last_pop(PankVm *vm) {
    return vm->last_pop;
}  // for testing -> see testmain.c
Module *get_cur_mod(PankVm *vm) { return vm->current_mod; }

void runtime_err(PankVm *vm, wchar_t *format, ...) {
    // setlocale(LC_CTYPE, "");
    va_list args;
    va_start(args, format);
    vfwprintf(stderr, format, args);
    // vwprintf(format, args)

    va_end(args);

    fputwc('\n', stderr);

    for (int i = get_cur_mod(vm)->frame_count - 1; i >= 0; i--) {
        CallFrame *frm = &get_cur_mod(vm)->frames[i];
        //&vm.frames[i];
        ObjFunc *fn = frm->closure->func;
        size_t inst = frm->ip - fn->ins.code - 1;  // vm.ip - vm.ins->code - 1;
        int line = fn->ins.lines[inst];            // vm.ins->lines[inst];
        fwprintf(stderr, L"Error [line %d] in \n", line);
        if (fn->name == NULL) {
            fwprintf(stderr, L"script\n");
        } else {
            fwprintf(stderr, L"%.*ls()", fn->name->len, fn->name->chars);
        }
    }

    reset_stack(vm);
}

bool push(PankVm *vm, Value value) {
    // cp_color_print('b' , L"++Pushing value -> ");
    // print_val(value);
    // cp_color_println('b', L"----");
    *vm->stack_top = value;
    vm->stack_top++;
    // if (is_err_obj(value)) {
    //     runtime_err(L"Error occured : %ls", get_as_err(value)->errmsg);
    //     return false;
    // }
    return true;
}

Value pop(PankVm *vm) {
    vm->stack_top--;
    return *vm->stack_top;
}

Value peek_vm(PankVm *vm, int dist) { return vm->stack_top[-1 - dist]; }

CallFrame *get_cur_farme(PankVm *vm) {
    return &get_cur_mod(vm)->frames[get_cur_mod(vm)->frame_count - 1];
}
uint8_t read_bt(CallFrame *frame) {
    uint8_t result = *frame->ip++;
    return result;
}
uint8_t peek_bt(CallFrame *frame) {
    uint8_t result = read_bt(frame);
    frame->ip--;
    return result;
}
uint16_t read_u16(CallFrame *frame) {
    CallFrame *cf = frame;  // get_cur_farme();
    cf->ip += 2;
    return (uint16_t)((cf->ip[-2] << 8) | cf->ip[-1]);
}
Value read_const(CallFrame *frame) {
    return frame->closure->func->ins.consts.values[read_bt(frame)];
}
ObjString *read_str_const(CallFrame *frame) {
    return get_as_string(read_const(frame));
}
void add_string(PankVm *vm) {
    ObjString *r = get_as_string(peek_vm(vm, 0));
    ObjString *l = get_as_string(peek_vm(vm, 1));

    int newlen = l->len + r->len;
    wchar_t *newchars = ALLOC(vm, wchar_t, newlen + 1);
    wmemcpy(newchars, l->chars, l->len);
    wmemcpy(newchars + l->len, r->chars, r->len);
    newchars[newlen] = '\0';
    ObjString *new_obj = take_string(vm, newchars, newlen);
    pop(vm);
    pop(vm);
    push(vm, make_obj_val(new_obj));
}

bool bin_add(PankVm *vm) {
    if (is_str_obj(peek_vm(vm, 0)) && is_str_obj(peek_vm(vm, 1))) {
        add_string(vm);
        return true;

    } else if (is_num(peek_vm(vm, 0)) && is_num(peek_vm(vm, 1))) {
        double r = get_as_number(pop(vm));
        double l = get_as_number(pop(vm));
        push(vm, make_num(l + r));
        return true;

    } else {
        runtime_err(vm,
                    L"Operands must be numbers or string for binary addition "
                    L"operation");
        return false;
    }
}

bool bin_sub(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(vm, L"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_num(l - r));
    return true;
}

bool bin_mul(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(vm, L"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_num(l * r));
    return true;
}

bool bin_div(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(vm, L"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_num(l / r));
    return true;
}

bool bin_gt(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(vm, L"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_bool(l > r));
    return true;
}

bool bin_gte(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(
            vm, L"Operands must be numbers for greater than equal operation");
        return false;
    }

    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_bool(l >= r));
    return true;
}

bool bin_lt(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(vm, L"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_bool(l < r));
    return true;
}

bool bin_lte(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(vm,
                    L"Operands must be numbers for less than equal operation");
        return false;
    }
    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_bool(l <= r));
    return true;
}

static int import_custom(PankVm *vm, wchar_t *custom_name,
                         wchar_t *import_name) {
    WSrcfile ws = wread_file(import_name);  // Warning import cycle

    if (ws.errcode != 0) {
        return ws.errcode;
    }
    Module *mod = &vm->modules[vm->mod_count++];
    mod->is_default = false;
    mod->origin = get_cur_mod(vm);

    init_module(mod, custom_name);

    int origin_caller = get_cur_mod(vm)->frame_count - 1;
    ObjMod *objmod = new_mod(vm, custom_name);

    push(vm, make_obj_val(objmod));  // peek 1
    // cp_println(L"-----> %ls", objmod->name->chars);
    vm->mod_names[vm->mod_count - 1] = objmod->name->hash;
    ObjString *strname = copy_string(vm, custom_name, wcslen(custom_name));
    push(vm, make_obj_val(strname));  // peek 0

    table_set(vm, &get_cur_mod(vm)->globals, get_as_string(peek_vm(vm, 0)),
              peek_vm(vm, 1));
    vm->current_mod = mod;  // vm.mod_count - 1;
    ObjFunc *newfn = compile_module(vm, ws.source);

    if (newfn == NULL) {
        return ERC_COMPTIME;
    }

    push(vm, make_obj_val(newfn));
    ObjClosure *cls = new_closure(vm, newfn, objmod->name->hash);
    cls->global_owner = objmod->name->hash;
    cls->globals = &mod->globals;
    pop(vm);
    pop(vm);
    pop(vm);
    call(vm, cls, origin_caller, 0);
    free(ws.source);
    return 0;
}

static int import_file(PankVm *vm, wchar_t *custom_name, wchar_t *import_name) {
    if (wcscmp(import_name, L"math") == 0) {
        ObjString *strname = copy_string(vm, custom_name, wcslen(custom_name));
        push(vm, make_obj_val(strname));
        ObjMod *objmod = new_mod(vm, custom_name);
        push(vm, make_obj_val(objmod));

        Module *mod = get_cur_mod(vm);
        table_set(vm, &mod->globals, get_as_string(peek_vm(vm, 1)),
                  peek_vm(vm, 0));

        StdProxy *prx = &mod->stdproxy[mod->stdlib_count++];

        prx->proxy_hash = objmod->name->hash;
        prx->proxy_name = objmod->name->chars;

        if (vm->stdlib_count < 1) {
            push_stdlib_math(vm);
            StdlibMod *sm = &vm->stdlibs[0];
            sm->owners[sm->owner_count++] = mod->hash;
            prx->origin_name = sm->name;
            prx->stdmod = sm;
        } else {
            for (int i = 0; i < vm->stdlib_count; i++) {
                StdlibMod *sm = &vm->stdlibs[i];
                if (sm->hash == objmod->name->hash) {
                    prx->origin_name = sm->name;
                    prx->stdmod = sm;
                    sm->owners[sm->owner_count++] = mod->hash;
                }
            }
        }

        pop(vm);
        pop(vm);
        return 0;

    } else {
        return import_custom(vm, custom_name, import_name);
    }
}

void print_stack(PankVm *vm) {
    wprintf(L"------ STACK ----\n");
    for (Value *slt = vm->stack; slt < vm->stack_top; slt++) {
        wprintf(L"[ ");
        print_val(*slt);
        wprintf(L" ]\n");
    }
    wprintf(L"--- END STACK ---\n");
}

IResult run_vm(PankVm *vm) {
    CallFrame *frame =
        &get_cur_mod(vm)->frames[get_cur_mod(vm)->frame_count - 1];
    for (;;) {
#ifdef DEBUG_STACK
        wprintf(L"------ STACK ----\n");
        for (Value *slt = vm.stack; slt < vm.stack_top; slt++) {
            wprintf(L"[ ");
            print_val(*slt);
            wprintf(L" ]\n");
        }
        wprintf(L"--- END STACK ---\n");
#endif
#ifdef DEBUG_TRACE

        dissm_ins(&frame->closure->func->ins,
                  (int)(frame->ip - frame->closure->func->ins.code));

#endif

        uint8_t ins = read_bt(frame);
        switch (ins) {
            case OP_END_MOD:
                pop(vm);  // in modules last nil still exists
                get_cur_mod(vm)->frame_count--;

                vm->current_mod = get_cur_mod(vm)->origin;

                frame =
                    &vm->current_mod->frames[vm->current_mod->frame_count - 1];
                continue;

            case OP_RETURN: {
                Value res = pop(vm);
                // cp_color_println('b', L"Return value ->");
                // print_val(peek_vm(0));
                //`cp_color_println('b', L"--------");

                close_upval(get_cur_mod(vm), frame->slots);

                get_cur_mod(vm)->frame_count--;

                if (get_cur_mod(vm)->frame_count == 0 &&
                    vm->current_mod->is_default) {
                    pop(vm);
                    return INTRP_OK;
                }

                vm->stack_top = frame->slots;
                if (!push(vm, res)) {
                    return INTRP_RUNTIME_ERR;
                }
                frame =
                    &get_cur_mod(vm)->frames[get_cur_mod(vm)->frame_count - 1];

                break;
            }
            case OP_CONST: {
                Value con = read_const(frame);
                if (!push(vm, con)) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_POP:
                pop(vm);
                break;
            case OP_NEG: {
                if (!is_num(peek_vm(vm, 0))) {
                    return INTRP_RUNTIME_ERR;
                }
                push(vm, make_neg(pop(vm)));
                break;
            }
            case OP_ADD:
                if (!bin_add(vm)) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            case OP_SUB:
                if (!bin_sub(vm)) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            case OP_MUL:
                if (!bin_mul(vm)) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            case OP_DIV:
                if (!bin_div(vm)) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            case OP_NIL:
                if (!push(vm, make_nil())) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            case OP_TRUE: {
                if (!push(vm, make_bool(true))) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_NOT: {
                bool b_val = is_falsey(pop(vm));
                if (!push(vm, make_bool(b_val))) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_EQ: {
                Value r = pop(vm);
                Value l = pop(vm);
                if (!push(vm, make_bool(is_equal(l, r))))
                    return INTRP_RUNTIME_ERR;
                break;
            }
            case OP_GT: {
                if (!bin_gt(vm)) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_GTE: {
                if (!bin_gte(vm)) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_LT: {
                if (!bin_lt(vm)) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }

            case OP_LTE: {
                if (!bin_lte(vm)) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_FALSE:
                if (!push(vm, make_bool(false))) return INTRP_RUNTIME_ERR;
                break;
            case OP_SHOW:
                cp_print(L"p~~ ");
                Value to_show = pop(vm);
                vm->last_pop = to_show;
                print_val(to_show);
                cp_print(L"\n");
                break;
            case OP_DEF_GLOB: {
                ObjString *nm = read_str_const(frame);
                table_set(vm, frame->globals, nm, peek_vm(vm, 0));
                pop(vm);
                break;
            }
            case OP_GET_GLOB: {
                ObjString *name = read_str_const(frame);
                Value val;
                if (!table_get(frame->globals, name, &val)) {
                    if (!table_get(&vm->builtins, name, &val)) {
                        runtime_err(vm,
                                    L"Get Global -> Undefined variable '%ls'.",
                                    name->chars);
                        return INTRP_RUNTIME_ERR;
                    }
                }
                if (!push(vm, val)) return INTRP_RUNTIME_ERR;
                break;
            }

            case OP_SET_GLOB: {
                ObjString *name = read_str_const(frame);
                // wprintf(L"setting global -> %ls -> %ld\n", name->chars,
                //         frame->global_owner);
                if (table_set(vm, frame->globals, name, peek_vm(vm, 0))) {
                    table_del(frame->globals, name);
                    runtime_err(vm, L"Set Global -> Undefined var '%ls'",
                                name->chars);
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }

            case OP_GET_LOCAL: {
                uint8_t slot = read_bt(frame);
                if (!push(vm, frame->slots[slot])) return INTRP_RUNTIME_ERR;
                break;
            }
            case OP_SET_LOCAL: {
                uint8_t slot = read_bt(frame);
                frame->slots[slot] = peek_vm(vm, 0);
                break;
            }
            case OP_JMP_IF_FALSE: {
                uint16_t offset = read_u16(frame);
                if (is_falsey(peek_vm(vm, 0))) {
                    frame->ip += offset;
                }
                break;
            }
            case OP_JMP: {
                uint16_t offset = read_u16(frame);
                frame->ip += offset;
                break;
            }
            case OP_LOOP: {
                uint16_t offset = read_u16(frame);
                frame->ip -= offset;
                break;
            }
            case OP_CLOSURE: {
                ObjFunc *func = get_as_func(read_const(frame));
                ObjClosure *cls = new_closure(vm, func, frame->global_owner);
                if (!push(vm, make_obj_val(cls))) return INTRP_RUNTIME_ERR;
                Module *mod = get_mod_by_hash(vm, frame->global_owner);
                for (int i = 0; i < cls->upv_count; i++) {
                    uint16_t is_local = read_bt(frame);
                    uint8_t index = read_bt(frame);
                    if (is_local) {
                        cls->upv[i] =
                            capture_upv(vm, mod, frame->slots + index);
                        // capture
                    } else {
                        cls->upv[i] = frame->closure->upv[index];
                    }
                }

                break;
            }
            case OP_GET_UP: {
                uint8_t slot = read_bt(frame);
                if (!push(vm, *frame->closure->upv[slot]->location))
                    return INTRP_RUNTIME_ERR;
                ;
                break;
            }
            case OP_SET_UP: {
                uint8_t slot = read_bt(frame);
                *frame->closure->upv[slot]->location = peek_vm(vm, 0);
                break;
            }
            case OP_CLS_UP: {
                Module *mod = get_mod_by_hash(vm, frame->global_owner);
                close_upval(mod, vm->stack_top - 1);
                pop(vm);
                break;
            }
            case OP_CALL: {
                int argc = read_bt(frame);
                if (!call_val(vm, peek_vm(vm, argc), argc)) {
                    return INTRP_RUNTIME_ERR;
                }

                frame =
                    &get_cur_mod(vm)->frames[get_cur_mod(vm)->frame_count - 1];
                break;
            }
            case OP_ARRAY: {
                int item_len = read_bt(frame);
                ObjArray *array = new_array(vm);
                push(vm, make_obj_val(array));
                for (int i = item_len; i > 0; i--) {
                    write_valarr(vm, &array->items, peek_vm(vm, i));
                }
                vm->stack_top -= item_len + 1;

                array->len = item_len;
                push(vm, make_obj_val(array));
                break;
            }
            case OP_ARR_INDEX: {
                Value raw_index = peek_vm(vm, 0);
                if (!is_num(raw_index)) {
                    runtime_err(vm, L"arrays can be only indexed with numbers");
                    return INTRP_RUNTIME_ERR;
                }
                double index = get_as_number(raw_index);
                if (index < 0 || ceil(index) != index) {
                    runtime_err(
                        vm, L"array index can only be non negetive integers");
                    return INTRP_RUNTIME_ERR;
                }
                Value raw_array = peek_vm(vm, 1);
                if (!is_array_obj(raw_array)) {
                    runtime_err(vm, L"only arrays can be indexed");
                    return INTRP_RUNTIME_ERR;
                }
                ObjArray *array = get_as_array(raw_array);
                if (index >= array->len) {
                    runtime_err(vm, L"Index out of range error");
                    return INTRP_RUNTIME_ERR;
                }

                Value val = array->items.values[(int)index];
                pop(vm);
                pop(vm);
                push(vm, val);

                break;
            }
            case OP_ERR: {
                Value msg = pop(vm);
                cp_print(L"Error : ");
                print_val(msg);
                cp_println(L"");
                runtime_err(vm, L"");
                return INTRP_RUNTIME_ERR;
            }
            case OP_IMPORT_NONAME: {
                // cp_color_println('b', L"IMPORT");
                Value raw_custom_name = read_const(frame);
                if (!is_str_obj(raw_custom_name)) {
                    runtime_err(vm, L"import custom name must be a identifier");
                    return INTRP_RUNTIME_ERR;
                }
                ObjString *custom_name = get_as_string(raw_custom_name);
                Value raw_file_name = pop(vm);
                // cp_println(L"VVV");
                // print_val(raw_file_name);

                // cp_println(L"\n^^^");
                if (!is_str_obj(raw_file_name)) {
                    runtime_err(vm, L"import file name must be string");
                    return INTRP_RUNTIME_ERR;
                }
                ObjString *filename = get_as_string(raw_file_name);
                int ir = import_file(vm, custom_name->chars, filename->chars);
                // cp_println(L"----> %ls|%ls| %d\n\n" , custom_name->chars ,
                // filename->chars , ir);

                if (ir != 0) {
                    switch (ir) {
                        case ERC_FAIL_TO_OPEN:
                            runtime_err(
                                vm,
                                L"Failed to open imported file! '%.*ls' "
                                L"doesn't "
                                L"exist!",
                                filename->len, filename->chars);
                            break;
                        case ERC_NO_MEM:
                            runtime_err(
                                vm,
                                L"Failed to open imported file '%ls'; ran out "
                                L"of memory while trying to read it!",
                                filename->chars);
                            break;
                        default:
                            runtime_err(
                                vm,
                                L"Faild to open imported file '%ls'; some "
                                L"unknown error occured! (code %d)",
                                filename->chars, ir);
                            break;
                    }
                    return INTRP_RUNTIME_ERR;
                }

                // print_table(&get_cur_mod()->globals, "GLOBALS");

                frame =
                    &get_cur_mod(vm)->frames[get_cur_mod(vm)->frame_count - 1];

                break;
            }

            case OP_GET_MOD_PROP: {
                if (!is_mod_obj(peek_vm(vm, 0))) {
                    runtime_err(vm, L"Module object is not a module");
                    return INTRP_RUNTIME_ERR;
                }
                ObjMod *modname = get_as_mod(peek_vm(vm, 0));

                // cp_color_println('r', L"modname -> %ls | %ld",
                // modname->name->chars , modname->name->hash);

                ObjString *prop = read_str_const(frame);
                // cp_color_println('b', L"prop -> %ls" , prop->chars);
                Value value;

                Module *mod = get_mod_by_hash(vm, modname->name->hash);

                if (mod == NULL) {
                    uint32_t modhash =

                        get_proxy_hash(modname->name->hash, get_cur_mod(vm));
                    // cp_color_println('g', L"proxy hash -> %ld" , modhash);
                    if (modhash != 0) {
                        StdlibMod *smod =
                            get_stdlib_by_hash(vm, modhash, get_cur_mod(vm));

                        //  cp_color_println('b', L"smod -> %ld" , smod->hash);
                        if (smod != NULL) {
                            if (table_get(&smod->items, prop, &value)) {
                                pop(vm);
                                if (!push(vm, value)) return INTRP_RUNTIME_ERR;
                                ;
                                break;
                            } else {
                                runtime_err(
                                    vm,
                                    L"cannot find method or variable '%ls' for "
                                    L"std "
                                    L"lib module '%ls'",
                                    prop->chars, modname->name);
                                return INTRP_RUNTIME_ERR;
                            }
                        }
                    }
                    runtime_err(vm, L"module not found\n");
                    print_obj(make_obj_val(modname->name));
                    return INTRP_RUNTIME_ERR;
                }

                if (table_get(&mod->globals, prop, &value)) {
                    pop(vm);
                    if (!push(vm, value)) return INTRP_RUNTIME_ERR;
                    ;
                    break;
                } else {
                    runtime_err(
                        vm,
                        L"Error method or variable '%ls' not found for module "
                        L"%ls",
                        prop->chars, modname->name);
                    return INTRP_RUNTIME_ERR;
                }
            }
        }
    }
    return INTRP_RUNTIME_ERR;
}

void close_upval(Module *module, Value *last) {
    while (module->open_upvs != NULL && module->open_upvs->location >= last) {
        ObjUpVal *upval = module->open_upvs;
        upval->closed = *upval->location;
        upval->location = &upval->closed;
        module->open_upvs = upval->next;
    }
}

ObjUpVal *capture_upv(PankVm *vm, Module *module, Value *local) {
    ObjUpVal *prev = NULL;
    ObjUpVal *upv = module->open_upvs;
    while (upv != NULL && upv->location > local) {
        prev = upv;
        upv = upv->next;
    }

    if (upv != NULL && upv->location == local) {
        return upv;
        ;
    }
    ObjUpVal *new_upv = new_up_val(vm, local);
    new_upv->next = upv;
    if (prev == NULL) {
        module->open_upvs = new_upv;
    } else {
        prev->next = new_upv;
    }
    return new_upv;
}

Value clock_ntv_fn(PankVm *vm, int argc, Value *args) {
    return make_num((double)clock() / CLOCKS_PER_SEC);
}

bool call_val(PankVm *vm, Value calle, int argc) {
    int origin = get_cur_mod(vm)->frame_count - 1;
    if (is_obj(calle)) {
        switch (get_obj_type(calle)) {
            case OBJ_CLOUSRE:
                return call(vm, get_as_closure(calle), origin, argc);
            case OBJ_NATIVE: {
                NativeFn native = get_as_native(calle);
                Value result = native(vm, argc, vm->stack_top - argc);
                vm->stack_top -= argc + 1;
                if (is_err_obj(result)) {
                    runtime_err(vm, get_as_err(result)->errmsg);
                    return false;
                }
                if (!push(vm, result)) return false;
                ;
                return true;
            }
            default:
                break;
        }
    }
    runtime_err(vm, L"can only call functions");
    return false;
}

bool call(PankVm *vm, ObjClosure *closure, int origin, int argc) {
    if (closure->func->arity != argc) {
        runtime_err(vm, L"Expected %d args but got %d", closure->func->arity,
                    argc);
        return false;
    }

    if (get_cur_mod(vm)->frame_count == FRAME_SIZE) {
        runtime_err(vm, L"Too many frames! Stack overflow");
        return false;
    }

    CallFrame *frame = &get_cur_mod(vm)->frames[get_cur_mod(vm)->frame_count++];
    frame->closure = closure;
    frame->ip = closure->func->ins.code;
    frame->slots = vm->stack_top - argc - 1;

    frame->global_owner = closure->global_owner;
    frame->globals = closure->globals;

    return true;
}

IResult interpret(PankVm *vm, wchar_t *source) {
    ObjFunc *fn = compile(vm, source);
    if (fn == NULL) {
        return INTRP_COMPILE_ERR;
    }

    if (!push(vm, make_obj_val(fn))) return INTRP_RUNTIME_ERR;
    ObjClosure *cls = new_closure(vm, fn, 0);
    pop(vm);
    if (!push(vm, make_obj_val(cls))) return INTRP_RUNTIME_ERR;
    cls->global_owner = 0;
    cls->globals = &vm->modules[0].globals;

    call(vm, cls, 0, 0);

    return run_vm(vm);
}

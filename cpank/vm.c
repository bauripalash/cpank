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

Vm vm;
const wchar_t *default_mod = L"__d__";
// #define DEBUG_STACK

void reset_stack() { vm.stack_top = vm.stack; }

void boot_vm() {
    reset_stack();
    gcon.is_paused = false;
    vm.objs = NULL;
    vm.last_pop = make_nil();

    vm.bts_allocated = 0;
    vm.next_gc = 1024 * 1024;
    vm.gray_cap = 0;
    vm.gray_count = 0;
    vm.gray_stack = NULL;
    vm.mod_count = 0;

    init_table(&vm.strings);
    init_table(&vm.builtins);

    vm.mod_names[vm.mod_count] = 0;
    Module *dmod = &vm.modules[vm.mod_count++];
    init_module(dmod, default_mod);
    dmod->is_default = true;
    dmod->origin = NULL;
    vm.mod_names[vm.mod_count] = get_hash(default_mod, wcslen(default_mod));
    vm.current_mod = dmod;
    define_native(L"clock", clock_ntv_fn);
}

void free_stdlibs() {
    for (int i = 0; i < vm.stdlib_count; i++) {
        StdlibMod *mod = &vm.stdlibs[i];
        if (mod->items.len > 0 || mod->items.cap > 0) {
            free_table(&mod->items);
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

Module *get_mod_by_hash(uint32_t hash) {
    for (int i = 0; i < vm.mod_count; i++) {
        if (vm.mod_names[i] == hash) {
            return &vm.modules[i];
        }
    }
    return NULL;
}

StdlibMod *get_stdlib_by_hash(uint32_t hash, Module *curmod) {
    uint32_t curmod_hash = get_hash(curmod->name, wcslen(curmod->name));
    for (int i = 0; i < vm.stdlib_count; i++) {
        StdlibMod *m = &vm.stdlibs[i];
        if (m->hash == hash) {
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

void print_mod_names() {
    for (int i = 0; i < vm.mod_count; i++) {
        wprintf(L"M| %4d -> %ld -> %ls \n", i, vm.mod_names[i],
                vm.modules[i].name);
    }
}
void free_module(Module *mod) {
    free_table(&mod->globals);

    mod->frame_count = 0;
    if (mod->source_code != NULL) {
        free(mod->source_code);
    }

    free(mod->name);
}

void define_native(wchar_t *name, NativeFn func) {
    push(make_obj_val(copy_string(name, (int)wcslen(name))));
    push(make_obj_val(new_native(func)));
    // Value k = make_obj_val(copy_string(name, (int)wcslen(name)));
    // Value fn = make_obj_val(new_native(func));
#ifdef DEBUG_STACK
    wprintf(L"------ STACK in Define native----\n");
    for (Value *slt = vm.stack; slt < vm.stack_top; slt++) {
        wprintf(L"[ ");
        print_val(*slt);
        wprintf(L" ]\n");
    }
    wprintf(L"--- END STACK ---\n");
#endif
    table_set(&vm.builtins, get_as_string(vm.stack[0]), vm.stack[1]);
    pop();
    pop();
}

void print_modframes() {
    for (int i = 0; i < vm.mod_count; i++) {
        Module *mod = &vm.modules[i];
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

void free_vm() {
    // wprintf(L"----------FREE VM----------\n");
    free_table(&vm.strings);
    free_table(&vm.builtins);
    free_stdlibs();
    for (int i = 0; i < vm.mod_count; i++) {
        free_module(&vm.modules[i]);
    }
    free_objs();

    // wprintf(L"----------DONE FREE VM----------\n");
}

Value get_last_pop() { return vm.last_pop; }
Module *get_cur_mod() { return vm.current_mod; }

void runtime_err(wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    vfwprintf(stderr, format, args);

    va_end(args);
    fputwc('\n', stderr);

    for (int i = get_cur_mod()->frame_count - 1; i >= 0; i--) {
        CallFrame *frm = &get_cur_mod()->frames[i];
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

    reset_stack();
}

bool push(Value value) {
    *vm.stack_top = value;
    vm.stack_top++;
    if (is_err_obj(value)) {
        runtime_err(L"Error occured : %ls", get_as_err(value)->msg->chars);
        return false;
    }
    return true;
}

Value pop() {
    vm.stack_top--;
    return *vm.stack_top;
}

Value peek_vm(int dist) { return vm.stack_top[-1 - dist]; }

CallFrame *get_cur_farme() {
    return &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];
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
void add_string() {
    ObjString *r = get_as_string(peek_vm(0));
    ObjString *l = get_as_string(peek_vm(1));

    int newlen = l->len + r->len;
    wchar_t *newchars = ALLOC(wchar_t, newlen + 1);
    wmemcpy(newchars, l->chars, l->len);
    wmemcpy(newchars + l->len, r->chars, r->len);
    newchars[newlen] = '\0';
    ObjString *new_obj = take_string(newchars, newlen);
    pop();
    pop();
    push(make_obj_val(new_obj));
}

bool bin_add() {
    if (is_str_obj(peek_vm(0)) && is_str_obj(peek_vm(1))) {
        add_string();
        return true;

    } else if (is_num(peek_vm(0)) && is_num(peek_vm(1))) {
        double r = get_as_number(pop());
        double l = get_as_number(pop());
        push(make_num(l + r));
        return true;

    } else {
        runtime_err(
            L"Operands must be numbers or string for binary addition "
            L"operation");
        return false;
    }
}

bool bin_sub() {
    if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
        runtime_err(L"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop());
    double l = get_as_number(pop());
    push(make_num(l - r));
    return true;
}

bool bin_mul() {
    if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
        runtime_err(L"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop());
    double l = get_as_number(pop());
    push(make_num(l * r));
    return true;
}

bool bin_div() {
    if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
        runtime_err(L"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop());
    double l = get_as_number(pop());
    push(make_num(l / r));
    return true;
}

bool bin_gt() {
    if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
        runtime_err(L"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop());
    double l = get_as_number(pop());
    push(make_bool(l > r));
    return true;
}

bool bin_gte() {
    if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
        runtime_err(
            L"Operands must be numbers for greater than equal operation");
        return false;
    }

    double r = get_as_number(pop());
    double l = get_as_number(pop());
    push(make_bool(l >= r));
    return true;
}

bool bin_lt() {
    if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
        runtime_err(L"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop());
    double l = get_as_number(pop());
    push(make_bool(l < r));
    return true;
}

bool bin_lte() {
    if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
        runtime_err(L"Operands must be numbers for less than equal operation");
        return false;
    }
    double r = get_as_number(pop());
    double l = get_as_number(pop());
    push(make_bool(l <= r));
    return true;
}

void define_stdlib_fn(wchar_t *name, NativeFn func) {
    // push(make_obj_val(copy_string(name, (int)wcslen(name))));
    // push(;
    Value k = make_obj_val(copy_string(name, (int)wcslen(name)));
    Value fn = make_obj_val(new_native(func));
#ifdef DEBUG_STACK
    wprintf(L"------ STACK in Define native----\n");
    for (Value *slt = vm.stack; slt < vm.stack_top; slt++) {
        wprintf(L"[ ");
        print_val(*slt);
        wprintf(L" ]\n");
    }
    wprintf(L"--- END STACK ---\n");
#endif
    table_set(&get_cur_mod()->globals, get_as_string(k), fn);
}

Value math_add_stdlib(int argc, Value *args) {
    if (argc != 2) {
        runtime_err(L"add function only takes two arguments");
        return make_nil();
    }

    if (is_num(args[0]) && is_num(args[1])) {
        double l = get_as_number(args[0]);
        double r = get_as_number(args[1]);
        return make_num(l + r);
    } else {
        runtime_err(L"add function only works on numbers");
        return make_nil();
    }
}
Value math_pow_stdlib(int argc, Value *args) {
    if (argc != 2) {
        runtime_err(L"pow function only takes two arguments");
        return make_nil();
    }

    if (is_num(args[0]) && is_num(args[1])) {
        double l = get_as_number(args[0]);
        double r = get_as_number(args[1]);

        return make_num(pow(l, r));
    } else {
        runtime_err(L"pow function only works on numbers");
        return make_nil();
    }
}

static int import_custom(wchar_t *custom_name, wchar_t *import_name) {
    WSrcfile ws = wread_file(import_name);  // Warning import cycle

    if (ws.errcode != 0) {
        return ws.errcode;
    }
    Module *mod = &vm.modules[vm.mod_count++];
    mod->is_default = false;
    mod->origin = get_cur_mod();

    init_module(mod, custom_name);
    // mod->source_code = ws.source;
    // wmemcpy(mod->source_code, ws.source, ws.size);

    int origin_caller = get_cur_mod()->frame_count - 1;
    ObjMod *objmod = new_mod(custom_name);
    vm.mod_names[vm.mod_count - 1] = objmod->name_hash;
    push(make_obj_val(objmod));
    ObjString *strname = copy_string(custom_name, wcslen(custom_name));

    table_set(&get_cur_mod()->globals, strname, make_obj_val(objmod));
    vm.current_mod = mod;  // vm.mod_count - 1;
    ObjFunc *newfn = compile_module(ws.source);
    if (newfn == NULL) {
        return ERC_COMPTIME;
    }

    push(make_obj_val(newfn));
    ObjClosure *cls = new_closure(newfn, objmod->name_hash);
    cls->global_owner = objmod->name_hash;
    cls->globals = &mod->globals;
    pop();
    call(cls, origin_caller, 0);
    free(ws.source);
    return 0;
}

static int import_file(wchar_t *custom_name, wchar_t *import_name) {
    if (wcscmp(import_name, L"math") == 0) {
        ObjMod *objmod = new_mod(custom_name);
        Module *mod = get_cur_mod();

        // Change later;

        StdProxy *prx = &get_cur_mod()->stdproxy[get_cur_mod()->stdlib_count++];
        prx->proxy_hash = objmod->name_hash;
        prx->proxy_name = objmod->name;

        if (vm.stdlib_count < 1) {
            push_stdlib_math();
            StdlibMod *sm = &vm.stdlibs[0];
            sm->owners[sm->owner_count++] = mod->hash;
            prx->origin_name = sm->name;
            prx->stdmod = sm;
        } else {
            for (int i = 0; i < vm.stdlib_count; i++) {
                StdlibMod *sm = &vm.stdlibs[i];
                if (sm->hash == objmod->name_hash) {
                    prx->origin_name = sm->name;
                    prx->stdmod = sm;
                    sm->owners[sm->owner_count++] = mod->hash;
                }
            }
        }

        ObjString *strname = copy_string(custom_name, wcslen(custom_name));

        table_set(&get_cur_mod()->globals, strname, make_obj_val(objmod));
        push_stdlib_math();
        return 0;

    } else {
        return import_custom(custom_name, import_name);
    }
}

void print_stack() {
    wprintf(L"------ STACK ----\n");
    for (Value *slt = vm.stack; slt < vm.stack_top; slt++) {
        wprintf(L"[ ");
        print_val(*slt);
        wprintf(L" ]\n");
    }
    wprintf(L"--- END STACK ---\n");
}

IResult run_vm() {
    CallFrame *frame = &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];
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
                pop();  // in modules last nil still exists
                get_cur_mod()->frame_count--;

                vm.current_mod = get_cur_mod()->origin;

                frame =
                    &vm.current_mod->frames[vm.current_mod->frame_count - 1];
                continue;

            case OP_RETURN: {
                Value res = pop();

                close_upval(get_cur_mod(), frame->slots);

                get_cur_mod()->frame_count--;

                if (get_cur_mod()->frame_count == 0 &&
                    vm.current_mod->is_default) {
                    pop();
                    return INTRP_OK;
                }

                vm.stack_top = frame->slots;
                if (!push(res)) {
                    return INTRP_RUNTIME_ERR;
                }
                frame = &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];

                break;
            }
            case OP_CONST: {
                Value con = read_const(frame);
                if (!push(con)) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_POP:
                pop();
                break;
            case OP_NEG: {
                if (!is_num(peek_vm(0))) {
                    return INTRP_RUNTIME_ERR;
                }
                push(make_neg(pop()));
                break;
            }
            case OP_ADD:
                if (!bin_add()) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            case OP_SUB:
                if (!bin_sub()) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            case OP_MUL:
                if (!bin_mul()) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            case OP_DIV:
                if (!bin_div()) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            case OP_NIL:
                if (!push(make_nil())) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            case OP_TRUE: {
                if (!push(make_bool(true))) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_NOT: {
                bool b_val = is_falsey(pop());
                if (!push(make_bool(b_val))) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_EQ: {
                Value r = pop();
                Value l = pop();
                if (!push(make_bool(is_equal(l, r)))) return INTRP_RUNTIME_ERR;
                break;
            }
            case OP_GT: {
                if (!bin_gt()) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_GTE: {
                if (!bin_gte()) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_LT: {
                if (!bin_lt()) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }

            case OP_LTE: {
                if (!bin_lte()) {
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }
            case OP_FALSE:
                if (!push(make_bool(false))) return INTRP_RUNTIME_ERR;
                break;
            case OP_SHOW:
                cp_print(L"p~~ ");
                Value to_show = pop();
                vm.last_pop = to_show;
                print_val(to_show);
                cp_print(L"\n");
                break;
            case OP_DEF_GLOB: {
                ObjString *nm = read_str_const(frame);
                table_set(frame->globals, nm, peek_vm(0));
                pop();
                break;
            }
            case OP_GET_GLOB: {
                ObjString *name = read_str_const(frame);
                Value val;
                if (!table_get(frame->globals, name, &val)) {
                    if (!table_get(&vm.builtins, name, &val)) {
                        runtime_err(L"Get Global -> Undefined variable '%ls'.",
                                    name->chars);
                        return INTRP_RUNTIME_ERR;
                    }
                }
                if (!push(val)) return INTRP_RUNTIME_ERR;
                break;
            }

            case OP_SET_GLOB: {
                ObjString *name = read_str_const(frame);
                wprintf(L"setting global -> %ls -> %ld\n", name->chars,
                        frame->global_owner);
                if (table_set(frame->globals, name, peek_vm(0))) {
                    table_del(frame->globals, name);
                    runtime_err(L"Set Global -> Undefined var '%ls'",
                                name->chars);
                    return INTRP_RUNTIME_ERR;
                }
                break;
            }

            case OP_GET_LOCAL: {
                uint8_t slot = read_bt(frame);
                if (!push(frame->slots[slot])) return INTRP_RUNTIME_ERR;
                break;
            }
            case OP_SET_LOCAL: {
                uint8_t slot = read_bt(frame);
                frame->slots[slot] = peek_vm(0);
                break;
            }
            case OP_JMP_IF_FALSE: {
                uint16_t offset = read_u16(frame);
                if (is_falsey(peek_vm(0))) {
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
                ObjClosure *cls = new_closure(func, frame->global_owner);
                if (!push(make_obj_val(cls))) return INTRP_RUNTIME_ERR;
                Module *mod = get_mod_by_hash(frame->global_owner);
                for (int i = 0; i < cls->upv_count; i++) {
                    uint16_t is_local = read_bt(frame);
                    uint8_t index = read_bt(frame);
                    if (is_local) {
                        cls->upv[i] = capture_upv(mod, frame->slots + index);
                        // capture
                    } else {
                        cls->upv[i] = frame->closure->upv[index];
                    }
                }

                break;
            }
            case OP_GET_UP: {
                uint8_t slot = read_bt(frame);
                if (!push(*frame->closure->upv[slot]->location))
                    return INTRP_RUNTIME_ERR;
                ;
                break;
            }
            case OP_SET_UP: {
                uint8_t slot = read_bt(frame);
                *frame->closure->upv[slot]->location = peek_vm(0);
                break;
            }
            case OP_CLS_UP: {
                Module *mod = get_mod_by_hash(frame->global_owner);
                close_upval(mod, vm.stack_top - 1);
                pop();
                break;
            }
            case OP_CALL: {
                int argc = read_bt(frame);
                if (!call_val(peek_vm(argc), argc)) {
                    return INTRP_RUNTIME_ERR;
                }

                frame = &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];
                break;
            }
            case OP_IMPORT_NONAME: {
                Value raw_custom_name = read_const(frame);
                if (!is_str_obj(raw_custom_name)) {
                    runtime_err(L"import custom name must be a identifier");
                    return INTRP_RUNTIME_ERR;
                }
                ObjString *custom_name = get_as_string(raw_custom_name);
                Value raw_file_name = pop();
                if (!is_str_obj(raw_file_name)) {
                    runtime_err(L"import file name must be string");
                    return INTRP_RUNTIME_ERR;
                }
                ObjString *filename = get_as_string(raw_file_name);
                int ir = import_file(custom_name->chars, filename->chars);

                if (ir != 0) {
                    switch (ir) {
                        case ERC_FAIL_TO_OPEN:
                            runtime_err(
                                L"Failed to open imported file! '%.*ls' "
                                L"doesn't "
                                L"exist!",
                                filename->len, filename->chars);
                            break;
                        case ERC_NO_MEM:
                            runtime_err(
                                L"Failed to open imported file '%ls'; ran out "
                                L"of memory while trying to read it!",
                                filename->chars);
                            break;
                        default:
                            runtime_err(
                                L"Faild to open imported file '%ls'; some "
                                L"unknown error occured! (code %d)",
                                filename->chars, ir);
                            break;
                    }
                    return INTRP_RUNTIME_ERR;
                }

                frame = &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];

                break;
            }
            case OP_GET_MOD_PROP: {
                ObjMod *modname = get_as_mod(peek_vm(0));

                cp_color_println('r', L"modname -> %d", modname->name_len);

                ObjString *prop = read_str_const(frame);
                Value value;

                Module *mod = get_mod_by_hash(modname->name_hash);

                if (mod == NULL) {
                    uint32_t modhash =

                        get_proxy_hash(modname->name_hash, get_cur_mod());
                    if (modhash != 0) {
                        StdlibMod *smod =
                            get_stdlib_by_hash(modhash, get_cur_mod());
                        if (smod != NULL) {
                            if (table_get(&smod->items, prop, &value)) {
                                pop();
                                if (!push(value)) return INTRP_RUNTIME_ERR;
                                ;
                                break;
                            } else {
                                runtime_err(
                                    L"cannot find method or variable '%ls' for "
                                    L"std "
                                    L"lib module '%ls'",
                                    prop->chars, modname->name);
                                return INTRP_RUNTIME_ERR;
                            }
                        }
                    }
                    runtime_err(L"module not found %ls\n", modname->name);
                    return INTRP_RUNTIME_ERR;
                }

                if (table_get(&mod->globals, prop, &value)) {
                    pop();
                    if (!push(value)) return INTRP_RUNTIME_ERR;
                    ;
                    break;
                } else {
                    runtime_err(
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

ObjUpVal *capture_upv(Module *module, Value *local) {
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
    ObjUpVal *new_upv = new_up_val(local);
    new_upv->next = upv;
    if (prev == NULL) {
        module->open_upvs = new_upv;
    } else {
        prev->next = new_upv;
    }
    return new_upv;
}

Value clock_ntv_fn(int argc, Value *args) {
    return make_num((double)clock() / CLOCKS_PER_SEC);
}

bool call_val(Value calle, int argc) {
    int origin = get_cur_mod()->frame_count - 1;
    if (is_obj(calle)) {
        switch (get_obj_type(calle)) {
            case OBJ_CLOUSRE:
                return call(get_as_closure(calle), origin, argc);
            case OBJ_NATIVE: {
                NativeFn native = get_as_native(calle);
                Value result = native(argc, vm.stack_top - argc);
                vm.stack_top -= argc + 1;
                if (!push(result)) return false;
                ;
                return true;
            }
            default:
                break;
        }
    }
    runtime_err(L"can only call functions");
    return false;
}

bool call(ObjClosure *closure, int origin, int argc) {
    if (closure->func->arity != argc) {
        runtime_err(L"Expected %d args but got %d", closure->func->arity, argc);
        return false;
    }

    if (get_cur_mod()->frame_count == FRAME_SIZE) {
        runtime_err(L"Too many frames! Stack overflow");
        return false;
    }

    CallFrame *frame = &get_cur_mod()->frames[get_cur_mod()->frame_count++];
    frame->closure = closure;
    frame->ip = closure->func->ins.code;
    frame->slots = vm.stack_top - argc - 1;

    frame->global_owner = closure->global_owner;
    frame->globals = closure->globals;

    return true;
}

IResult interpret(wchar_t *source) {
    ObjFunc *fn = compile(source);
    if (fn == NULL) {
        return INTRP_COMPILE_ERR;
    }

    if (!push(make_obj_val(fn))) return INTRP_RUNTIME_ERR;
    ObjClosure *cls = new_closure(fn, 0);
    pop();
    if (!push(make_obj_val(cls))) return INTRP_RUNTIME_ERR;
    cls->global_owner = 0;
    cls->globals = &vm.modules[0].globals;

    call(cls, 0, 0);

    return run_vm();
}

/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

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
#include <uchar.h>
#include <wchar.h>

#include "include/builtins.h"
#include "include/common.h"
#include "include/compiler.h"
#include "include/debug.h"
#include "include/helper/os.h"
#include "include/htable.h"
#include "include/instruction.h"
#include "include/mem.h"
#include "include/obj.h"
#include "include/openfile.h"
#include "include/stdlib.h"
#include "include/utils.h"
#include "include/value.h"

// #define DEBUG_STACK

bool init_pbuffer(PrintBuffer *buffer) {
    buffer->buff = (char32_t *)calloc(PBUFFER_INIT_SIZE, sizeof(char32_t));
    if (buffer->buff == NULL) {
        return false;
    }
    buffer->cap = PBUFFER_INIT_SIZE;
    buffer->len = 0;
    buffer->ptr = buffer->buff;
    return true;
}

bool init_zero_pbuffer(PrintBuffer *buffer) {
    buffer->buff = NULL;
    buffer->cap = 0;
    buffer->len = 0;
    buffer->ptr = NULL;
    return true;
}

bool free_pbuffer(PrintBuffer *buffer) {
    free(buffer->buff);
    buffer->cap = 0;
    buffer->len = 0;
    buffer->ptr = NULL;
    return true;
}

int grow_pbuffer(PrintBuffer *buffer, int atleast) {
    int newcap = (buffer->cap + atleast + 1) * PBUFFER_GROW_BY;
    buffer->buff = realloc(buffer->buff, newcap * sizeof(char32_t));
    buffer->cap = newcap;
    return newcap;
}

int write_pbuffer(PrintBuffer *buffer, char *fmt, ...) {
    if (buffer->ptr == NULL) {
        return 0;
    }
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    va_start(ap, fmt);
    char *temp_buff = calloc(len + 1, sizeof(char));
    if (temp_buff == NULL) {
        va_end(ap);
        return 0;
    }
    // int avil = buffer->cap - buffer->len;
    int w = vsnprintf(temp_buff, len + 1, fmt, ap);
    if (w <= 0) {
        free(temp_buff);
        return 0;
    }
    char32_t *temp32buff = char_to_32(temp_buff);
    int tbsz = strlen32(temp32buff);
    if (tbsz + buffer->len + 1 > buffer->cap) {
        grow_pbuffer(buffer, tbsz + 1);
    }

    copy_c32(buffer->ptr, temp32buff, tbsz);
    free(temp_buff);
    free(temp32buff);
    buffer->ptr += tbsz;
    buffer->len += tbsz;
    return tbsz;
}

int write_pbuffer_with_arglist(PrintBuffer *buffer, char *fmt, va_list ap,
                               int len) {
    char *temp_buff = calloc(len + 1, sizeof(char));
    if (temp_buff == NULL) {
        return 0;
    }

    int w = vsnprintf(temp_buff, len + 1, fmt, ap);
    if (w <= 0) {
        free(temp_buff);
        return 0;
    }

    char32_t *temp32buff = char_to_32(temp_buff);
    int tbsz = strlen32(temp32buff);
    if (tbsz + buffer->len + 1 > buffer->cap) {
        grow_pbuffer(buffer, tbsz + 1);
    }

    copy_c32(buffer->ptr, temp32buff, tbsz);
    free(temp_buff);
    free(temp32buff);
    buffer->ptr += tbsz;
    buffer->len += tbsz;
    return tbsz;
}

// cppcheck-suppress unusedFunction
void print_pbuffer(PrintBuffer *buffer) {
    if (buffer->ptr != NULL && buffer->len > 0) {
        cp_print(L"BUFF->%.*ls", buffer->len, buffer->buff);
    }
}

char32_t *get_trimmed(PrintBuffer *buffer) {
    char32_t *result = (char32_t *)calloc(buffer->len + 1, sizeof(char32_t));
    char32_t *ptr = result;
    copy_c32(ptr, buffer->buff, buffer->len);
    ptr += buffer->len;
    *ptr = U'\0';
    return result;
}

// PankVm vm;
const char32_t *default_mod = U"_d_";
// #define DEBUG_STACK

void reset_stack(PankVm *vm) { vm->stack_top = vm->stack; }
ObjUpVal *capture_upv(PankVm *vm, Module *module, Value *local);
void close_upval(Module *module, Value *last);
bool call_val(PankVm *vm, Value calle, int argc);
bool call(PankVm *vm, ObjClosure *closure, int origin, int argc);

PankVm *boot_vm(bool need_buffer) {
    PankVm *vm = malloc(sizeof(PankVm));
    memset(vm, 0, sizeof(PankVm));
    reset_stack(vm);
    gcon.is_paused = false;
    vm->objs = NULL;
    vm->last_pop = make_nil;
    vm->need_buffer = need_buffer;

    if (need_buffer) {
        init_pbuffer(&vm->buffer);
    } else {
        init_zero_pbuffer(&vm->buffer);
    }

    vm->bts_allocated = 0;
    vm->next_gc = 1024 * 1024;
    vm->gray_cap = 0;
    vm->gray_count = 0;
    vm->gray_stack = NULL;
    vm->mod_count = 0;
    vm->stdlib_count = 0;

    init_table(&vm->strings);
    init_table(&vm->builtins);

    vm->mod_names[vm->mod_count] = 0;
    Module *dmod = &vm->modules[vm->mod_count++];
    init_module(dmod, default_mod);
    dmod->is_default = true;
    dmod->origin = NULL;
    vm->mod_names[vm->mod_count] = get_hash(default_mod, strlen32(default_mod));
    vm->current_mod = dmod;
    define_native(vm, clock_ntv_name, clock_ntv_fn);
    define_native(vm, asserteq_ntv_name, asserteq_ntv_fn);
    define_native(vm, type_ntv_name, type_ntv_fn);
    define_native(vm, bn_native_fn_name, bn_type_ntv_fn);
    define_native(vm, len_ntv_name, len_ntv_fn);
    define_native(vm, bignew_ntv_name, bignew_ntv_fn);
    // write_pbuffer(&vm->buffer, "hello world %ls" , L"how are you\n");
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

bool is_stdlib(char32_t *name) {
    return str32cmp(name, STDOS) || str32cmp(name, STDMATH) ||
           str32cmp(name, STDMATH_BN) || str32cmp(name, STDCOMMON) ||
           str32cmp(name, STDARRAY) || str32cmp(name, STDSTR) ||
           str32cmp(name, STDFILE) || str32cmp(name, STDHMAP) ||
           str32cmp(name, STDOS_BN);
}

void init_module(Module *mod, const char32_t *name) {
    init_table(&mod->globals);
    // init_stdlib_module();
    mod->frame_count = 0;
    mod->stdlib_count = 0;
    mod->hash = get_hash(name, strlen32(name));
    mod->name = malloc(sizeof(wchar_t) * (strlen32(name) + 1));
    memcpy(mod->name, name, strlen32(name) + 1);
    mod->open_upvs = NULL;
    mod->source_code = NULL;
}

// cppcheck-suppress unusedFunction
bool is_default(Module *mod) {
    return memcmp(mod->name, default_mod, strlen32(default_mod)) == 0 &&
           mod->is_default;
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

// cppcheck-suppress unusedFunction
void print_mod_names(PankVm *vm) {
    for (int i = 0; i < vm->mod_count; i++) {
        cp_println(L"M| %4d -> %ld -> %ls", i, vm->mod_names[i],
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

void define_native(PankVm *vm, char32_t *name, NativeFn func) {
    push(vm, make_obj_val(copy_string(vm, name, strlen32(name))));  // peek 1
    push(vm, make_obj_val(new_native(vm, func, name)));             // peek 0
    table_set(vm, &vm->builtins, get_as_string(vm->stack[0]), vm->stack[1]);
    pop(vm);
    pop(vm);
}

// cppcheck-suppress unusedFunction
void print_modframes(PankVm *vm) {
    for (int i = 0; i < vm->mod_count; i++) {
        Module *mod = &vm->modules[i];
        wprintf(L"module %ls has %d frames \n", mod->name, mod->frame_count);
        for (int f = 0; f < mod->frame_count; f++) {
            CallFrame *fr = &mod->frames[f];
            char32_t *name = U"unknown";
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
    if (vm->need_buffer) {
        char32_t *res = get_trimmed(&vm->buffer);
        cp_print(L"%ls", res);
        free(res);
        // print_pbuffer(&vm->buffer);
        free_pbuffer(&vm->buffer);
    }
    free(vm);
}

// cppcheck-suppress unusedFunction
Value get_last_pop(PankVm *vm) {
    return vm->last_pop;
}  // for testing -> see testmain.c

Module *get_cur_mod(PankVm *vm) { return vm->current_mod; }

void runtime_err(PankVm *vm, char32_t *format, ...) {
    char *strformat = c_to_c(format, 0);
    // setlocale(LC_CTYPE, "");
    va_list args;
    va_start(args, format);

    // vfwprintf(stderr, format, args);
    if (vm->need_buffer) {
        int len = vsnprintf(NULL, 0, strformat, args);
        va_end(args);
        va_start(args, format);
        write_pbuffer_with_arglist(&vm->buffer, strformat, args, len);
        va_end(args);
        va_start(args, format);
        // write_pbuffer(PrintBuffer *buffer, char *fmt, ...)
    } else {
        vfprintf(stderr, strformat, args);
    }
    // vwprintf(format, args)

    va_end(args);

    fputwc('\n', stderr);

    for (int i = get_cur_mod(vm)->frame_count - 1; i >= 0; i--) {
        CallFrame *frm = &get_cur_mod(vm)->frames[i];
        //&vm.frames[i];
        ObjFunc *fn = frm->closure->func;
        size_t inst = frm->ip - fn->ins.code - 1;  // vm.ip - vm.ins->code - 1;
        int line = fn->ins.lines[inst];            // vm.ins->lines[inst];
        if (vm->need_buffer) {
            write_pbuffer(&vm->buffer, "Error [line %d] in \n", line);
        } else {
            fwprintf(stderr, L"Error [line %d] in \n", line);
        }
        if (fn->name == NULL) {
            if (vm->need_buffer) {
                write_pbuffer(&vm->buffer, "script\n");
            } else {
                fwprintf(stderr, L"script\n");
            }
        } else {
            if (vm->need_buffer) {
                write_pbuffer(&vm->buffer, "%.*ls", fn->name->len,
                              fn->name->chars);
            } else {
                fwprintf(stderr, L"%.*ls()", fn->name->len, fn->name->chars);
            }
        }
    }

    free(strformat);

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

/*CallFrame *get_cur_farme(PankVm *vm) {
    return &get_cur_mod(vm)->frames[get_cur_mod(vm)->frame_count - 1];
}*/
uint8_t read_bt(CallFrame *frame) {
    uint8_t result = *frame->ip++;
    return result;
}

/*uint8_t peek_bt(CallFrame *frame) {
    uint8_t result = read_bt(frame);
    frame->ip--;
    return result;
}*/

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

    char *l_str = c_to_c(l->chars, 0);
    char *r_str = c_to_c(r->chars, 0);
    size_t new_size = sizeof(char) * (strlen(l_str) + strlen(r_str) + 1);
    char *new_str = (char *)malloc(new_size);
    if (new_str == NULL) {
        runtime_err(
            vm,
            U"Failed to allocate memory for joining strings\n Internal Error");
        return;
    }
    memset(new_str, 0, new_size);
    strcat(new_str, l_str);
    strcat(new_str, r_str);
    char32_t *result_str = char_to_32(new_str);
    int ssz = strlen32(result_str);

    free(l_str);
    free(r_str);
    free(new_str);

    ObjString *new_obj = take_string(vm, result_str, ssz);
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
                    U"Operands must be numbers or string for binary addition "
                    U"operation");
        return false;
    }
}

bool bin_sub(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(vm, U"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_num(l - r));
    return true;
}

bool bin_mul(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(vm, U"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_num(l * r));
    return true;
}

bool bin_div(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(vm, U"Operands must be numbers for binary operation");
        return false;
    }
    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_num(l / r));
    return true;
}

bool bin_gt(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(vm, U"Operands must be numbers for binary operation");
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
            vm, U"Operands must be numbers for greater than equal operation");
        return false;
    }

    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_bool(l >= r));
    return true;
}

bool bin_lt(PankVm *vm) {
    if (!is_num(peek_vm(vm, 0)) || !is_num(peek_vm(vm, 1))) {
        runtime_err(vm, U"Operands must be numbers for binary operation");
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
                    U"Operands must be numbers for less than equal operation");
        return false;
    }
    double r = get_as_number(pop(vm));
    double l = get_as_number(pop(vm));
    push(vm, make_bool(l <= r));
    return true;
}

static int import_custom(PankVm *vm, char32_t *custom_name,
                         char32_t *import_name) {
    // WSrcfile ws = wread_file(import_name);  // Warning import cycle
    Srcfile ws = read_file(c_to_c(import_name, strlen32(import_name)));
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
    ObjString *strname = copy_string(vm, custom_name, strlen32(custom_name));
    push(vm, make_obj_val(strname));  // peek 0

    table_set(vm, &get_cur_mod(vm)->globals, get_as_string(peek_vm(vm, 0)),
              peek_vm(vm, 1));
    vm->current_mod = mod;  // vm.mod_count - 1;
    ObjFunc *newfn = compile_module(vm, char_to_32(ws.source));

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

void add_stdlib(PankVm *vm, char32_t *import_name) {
    if (str32cmp(import_name, STDMATH)) {
        push_stdlib_math(vm);
    } else if (str32cmp(import_name, STDMATH_BN)) {
        push_stdlib_math_bn(vm);
    } else if (str32cmp(import_name, STDOS)) {
        push_stdlib_os(vm);
    } else if (str32cmp(import_name, STDCOMMON)) {
        push_stdlib_common(vm);
    } else if (str32cmp(import_name, STDARRAY)) {
        push_stdlib_array(vm);
    } else if (str32cmp(import_name, STDSTR)) {
        push_stdlib_string(vm);
    } else if (str32cmp(import_name, STDFILE)) {
        push_stdlib_file(vm);
    } else if (str32cmp(import_name, STDHMAP)) {
        push_stdlib_map(vm);
    } else if (str32cmp(import_name, STDOS_BN)) {
        push_stdlib_os_bn(vm);
    }
}

static int import_file(PankVm *vm, char32_t *custom_name,
                       char32_t *import_name) {
    if (is_stdlib(import_name)) {
        ObjString *strname =
            copy_string(vm, custom_name, strlen32(custom_name));
        push(vm, make_obj_val(strname));
        // cp_println(L"--->>>>> %ls" , custom_name);
        ObjMod *objmod = new_mod(vm, custom_name);
        // print_stack(vm);
        push(vm, make_obj_val(objmod));
        // cp_println(L"-----------------------------");
        // print_val(peek_vm(vm, 1));

        // cp_println(L"\n-----------------------------");

        Module *mod = get_cur_mod(vm);
        table_set(vm, &mod->globals, get_as_string(peek_vm(vm, 1)),
                  peek_vm(vm, 0));
        //        print_table(&mod->globals , "globals");
        StdProxy *prx = &mod->stdproxy[mod->stdlib_count++];

        prx->proxy_hash = objmod->name->hash;
        prx->proxy_name = objmod->name->chars;

        if (vm->stdlib_count < 1) {
            add_stdlib(vm, import_name);

            StdlibMod *sm = &vm->stdlibs[0];
            sm->owners[sm->owner_count++] = mod->hash;
            prx->origin_name = sm->name;

            prx->stdmod = sm;
        } else {
            bool found = false;
            for (int i = 0; i < vm->stdlib_count; i++) {
                StdlibMod *sm = &vm->stdlibs[i];
                if (sm->hash == objmod->name->hash) {
                    found = true;
                    prx->origin_name = sm->name;
                    prx->stdmod = sm;
                    sm->owners[sm->owner_count++] = mod->hash;
                    // cp_color_println('r', L"SM -> %s", sm->name);
                    break;
                }
            }
            if (!found) {
                add_stdlib(vm, import_name);

                StdlibMod *sm = &vm->stdlibs[vm->stdlib_count - 1];
                sm->owners[sm->owner_count++] = mod->hash;
                prx->origin_name = sm->name;

                prx->stdmod = sm;
            }
        }

        //// cp_println(L"count -> %d" , vm->stdlib_count);
        // cp_println(L"---> peek0 ");
        // print_val(peek_vm(vm, 0));
        // print_stack(vm);
        pop(vm);
        pop(vm);
        return 0;

    } else {
        return import_custom(vm, custom_name, import_name);
    }
}

void print_stack(PankVm *vm) {  // cppcheck-suppress unusedFunction
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
        for (Value *slt = vm->stack; slt < vm->stack_top; slt++) {
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
        // print_stack(vm);

        // cp_println(L"]]]]]]]]]] OPCODE -> %s" , print_opcode(ins));
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
                if (!push(vm, make_nil)) {
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
            case OP_SHOW: {
                Value to_show = pop(vm);
                vm->last_pop = to_show;
                if (vm->need_buffer) {
                    char32_t *vl = value_to_string(vm, to_show);
                    write_pbuffer(&vm->buffer, "p~~ %ls\n", vl);
                    free(vl);
                } else {
#if defined(DEBUG)

                    cp_print(L"p~~ ");
#endif
                    print_val(to_show);
                    cp_print(L"\n");
                }
                break;
            }
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
                                    U"Get Global -> Undefined variable '%ls'.",
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
                    runtime_err(vm, U"Set Global -> Undefined var '%ls'",
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
            case OP_HMAP: {
                int len = read_bt(frame);
                ObjHashMap *hmap = new_hmap(vm);
                push(vm, make_obj_val(hmap));
                for (int i = len * 2; i > 0; i -= 2) {
                    Value key = peek_vm(vm, i);
                    // print_val(key);
                    if (!is_valid_hashmap_key(key)) {
                        runtime_err(
                            vm,
                            U"this value can not be used as key for hashmaps");
                        return INTRP_RUNTIME_ERR;
                    }
                    hmap_set(vm, hmap, key, peek_vm(vm, i - 1));
                }

                vm->stack_top -= len * 2 + 1;
                push(vm, make_obj_val(hmap));
                break;
            }
            case OP_ARR_INDEX: {
                Value raw_index = peek_vm(vm, 0);
                Value raw_obj = peek_vm(vm, 1);
                if (is_str_obj(raw_obj)) {
                    if (!is_num(raw_index)) {
                        runtime_err(
                            vm, U"Strings can only be indexed with numbers");
                        return INTRP_RUNTIME_ERR;
                    }
                    double index = get_as_number(raw_index);
                    if (index < 0 && ceil(index) != index) {
                        runtime_err(vm,
                                    U"strings can be only indexed with "
                                    U"non-negetive integers");
                        return INTRP_RUNTIME_ERR;
                    }
                    ObjString *s = get_as_string(raw_obj);
                    if (index >= s->len) {
                        runtime_err(
                            vm, U"index out of range; max index = %d ; got %d",
                            s->len - 1, (int)index);
                        return INTRP_RUNTIME_ERR;
                    }
                    char32_t item = s->chars[(int)index];
                    ObjString *newobjstring = copy_string(vm, &item, 1);
                    pop(vm);
                    pop(vm);
                    push(vm, make_obj_val(newobjstring));
                } else if (is_array_obj(raw_obj)) {
                    if (!is_num(raw_index)) {
                        runtime_err(vm,
                                    U"arrays can be only indexed with numbers");
                        return INTRP_RUNTIME_ERR;
                    }
                    double index = get_as_number(raw_index);
                    if (index < 0 || ceil(index) != index) {
                        runtime_err(
                            vm,
                            U"array index can only be non negetive integers");
                        return INTRP_RUNTIME_ERR;
                    }
                    ObjArray *array = get_as_array(raw_obj);
                    if (index >= array->items.len) {
                        runtime_err(vm, U"Index out of range error");
                        return INTRP_RUNTIME_ERR;
                    }

                    Value val = array->items.values[(int)index];
                    pop(vm);
                    pop(vm);
                    push(vm, val);
                } else if (is_map_obj(raw_obj)) {
                    ObjHashMap *hmap = get_as_hmap(raw_obj);
                    Value val;
                    bool found = hmap_get(hmap, raw_index, &val);
                    if (!found) {
                        runtime_err(vm, U"key not found in hashmap");
                        return INTRP_RUNTIME_ERR;
                    }
                    pop(vm);
                    pop(vm);
                    push(vm, val);
                } else {
                    runtime_err(vm, U"Only arrays and hashmaps can be indexed");
                    return INTRP_RUNTIME_ERR;
                }

                break;
            }
            case OP_ERR: {
                Value msg = pop(vm);
                cp_print(L"Error : ");
                print_val(msg);
                cp_println(L"");
                runtime_err(vm, U"");
                return INTRP_RUNTIME_ERR;
            }
            case OP_IMPORT_NONAME: {
                // cp_color_println('b', L"IMPORT");
                Value raw_custom_name = read_const(frame);
                if (!is_str_obj(raw_custom_name)) {
                    runtime_err(vm, U"import custom name must be a identifier");
                    return INTRP_RUNTIME_ERR;
                }
                ObjString *custom_name = get_as_string(raw_custom_name);
                Value raw_file_name = pop(vm);
                // cp_println(L"VVV");
                // print_val(raw_file_name);

                // cp_println(L"\n^^^");
                if (!is_str_obj(raw_file_name)) {
                    runtime_err(vm, U"import file name must be string");
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
                                U"Failed to open imported file! '%.*ls' "
                                U"doesn't "
                                U"exist!",
                                filename->len, filename->chars);
                            break;
                        case ERC_NO_MEM:
                            runtime_err(
                                vm,
                                U"Failed to open imported file '%ls'; ran out "
                                U"of memory while trying to read it!",
                                filename->chars);
                            break;
                        default:
                            runtime_err(
                                vm,
                                U"Faild to open imported file '%ls'; some "
                                U"unknown error occured! (code %d)",
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
            case OP_SUBSCRIPT_ASSIGN: {
                Value newval = peek_vm(vm, 0);     // the value
                Value raw_index = peek_vm(vm, 1);  // the index
                Value raw_main_obj =
                    peek_vm(vm, 2);  // the object -> Array / Hashmap
                /*cp_println(L"objval");
                print_val(newval);
                cp_println(L"\nindex");
                print_val(raw_index);
                cp_println(L"\nsubval");
                print_val(raw_main_obj);*/

                if (!is_map_obj(raw_main_obj) && !is_array_obj(raw_main_obj)) {
                    runtime_err(vm,
                                U"subscript assignment only works on arrays "
                                U"and hashmaps");
                    return INTRP_RUNTIME_ERR;
                }

                if (is_map_obj(raw_main_obj)) {
                    ObjHashMap *hmap = get_as_hmap(raw_main_obj);
                    if (!is_valid_hashmap_key(raw_index)) {
                        runtime_err(vm, U"Hashmap key is invalid type");
                        return INTRP_RUNTIME_ERR;
                    }
                    hmap_set(vm, hmap, raw_index, newval);
                    pop(vm);
                    pop(vm);
                    pop(vm);
                    push(vm, make_nil);
                } else if (is_array_obj(raw_main_obj)) {
                    if (!is_num(raw_index)) {
                        runtime_err(vm, U"array index must be number");
                        return INTRP_RUNTIME_ERR;
                    }

                    double raw_index_num = get_as_number(raw_index);
                    if (raw_index_num < 0 || !is_int(raw_index_num)) {
                        runtime_err(
                            vm, U"array index must be a non negetive integer");
                        return INTRP_RUNTIME_ERR;
                    }

                    int index = (int)raw_index_num;

                    ObjArray *arr = get_as_array(raw_main_obj);

                    if (index >= arr->items.len) {
                        runtime_err(vm, U"index out of range of array");
                        return INTRP_RUNTIME_ERR;
                    }

                    arr->items.values[index] = newval;
                    pop(vm);
                    pop(vm);
                    pop(vm);
                    push(vm, make_nil);
                }

                break;
            }

            case OP_GET_MOD_PROP: {
                // print_val(peek_vm(vm, 0));
                if (!is_mod_obj(peek_vm(vm, 0))) {
                    runtime_err(vm, U"Module object is not a module");
                    return INTRP_RUNTIME_ERR;
                }
                ObjMod *modname = get_as_mod(peek_vm(vm, 0));

                // cp_color_println('r', L"modname ->
                // %ls",modname->name->chars);
                //  modname->name->chars , modname->name->hash);

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
                                    U"cannot find method or variable '%ls' for "
                                    U"std "
                                    U"lib module '%ls'",
                                    prop->chars, modname->name);
                                return INTRP_RUNTIME_ERR;
                            }
                        }
                    }
                    runtime_err(vm, U"module not found\n");
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
                        U"Error method or variable '%ls' not found for module "
                        U"%ls",
                        prop->chars, modname->name);
                    return INTRP_RUNTIME_ERR;
                }
            }
        }
    }
    return INTRP_RUNTIME_ERR;
}

// cppcheck-suppress constParameter
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
                    ObjErr *er = get_as_err(result);
                    char32_t *msg32 = char_to_32(er->errmsg);
                    runtime_err(vm, msg32);
                    free(msg32);
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
    runtime_err(vm, U"can only call functions");
    return false;
}

bool call(PankVm *vm, ObjClosure *closure, int origin, int argc) {
    if (closure->func->arity != argc) {
        runtime_err(vm, U"Expected %d args but got %d", closure->func->arity,
                    argc);
        return false;
    }

    if (get_cur_mod(vm)->frame_count == FRAME_SIZE) {
        runtime_err(vm, U"Too many frames! Stack overflow");
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

IResult interpret(PankVm *vm, char32_t *source) {
    ObjFunc *fn = compile(vm, source);
    if (fn == NULL) {
        return INTRP_COMPILE_ERR;
    }

    // if (!dump_instruction(&fn->ins, "script.cpnk")) {
    //     cp_println(L"failed dump compiled instruction");
    // }

    if (!push(vm, make_obj_val(fn))) return INTRP_RUNTIME_ERR;
    ObjClosure *cls = new_closure(vm, fn, 0);
    pop(vm);
    if (!push(vm, make_obj_val(cls))) return INTRP_RUNTIME_ERR;
    cls->global_owner = 0;
    cls->globals = &vm->modules[0].globals;

    call(vm, cls, 0, 0);

    return run_vm(vm);
}

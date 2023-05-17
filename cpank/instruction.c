/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "include/instruction.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "include/mem.h"
#include "include/value.h"
#include "include/vm.h"

void init_instruction(Instruction *ins) {
    ins->len = 0;
    ins->cap = 0;
    ins->code = NULL;
    ins->lines = NULL;
    init_valarr(&ins->consts);
    init_iparr(&ins->posarr);
}

void free_ins(PankVm *vm, Instruction *ins) {
    FREE_ARR(vm, uint8_t, ins->code, ins->cap);
    FREE_ARR(vm, int, ins->lines, ins->cap);
    free_valarr(vm, &ins->consts);
    free_iparr(vm, &ins->posarr);
    init_instruction(ins);
}

void write_ins(PankVm *vm, Instruction *ins, uint8_t bt, int lines) {
    if (ins->cap < ins->len + 1) {
        int old_cap = ins->cap;
        ins->cap = GROW_CAP(old_cap);
        ins->code = GROW_ARR(vm, uint8_t, ins->code, old_cap, ins->cap);
        ins->lines = GROW_ARR(vm, int, ins->lines, old_cap, ins->cap);
    }

    ins->code[ins->len] = bt;
    ins->lines[ins->len] = lines;
    ins->len++;
}

int add_const(PankVm *vm, Instruction *ins, Value val) {
    push(vm, val);
    write_valarr(vm, &ins->consts, val);
    pop(vm);
    return ins->consts.len - 1;
}

void init_iparr(InstPosArray *arr) {
    arr->p = NULL;
    arr->len = 0;
    arr->cap = 0;
}
void free_iparr(PankVm *vm, InstPosArray *arr) {
    FREE_ARR(vm, InstPos, arr->p, arr->cap);
    init_iparr(arr);
}
void add_iparr(PankVm *vm, InstPosArray *arr, InstPos pos) {
    if (arr->cap < arr->len + 1) {
        int ocap = arr->cap;
        arr->cap = GROW_CAP(ocap);
        arr->p = GROW_ARR(vm, InstPos, arr->p, ocap, arr->cap);
    }

    arr->p[arr->len] = pos;
    arr->len++;
}
InstPos fetch_iparr(InstPosArray *arr, int index) {
    if (arr->len >= index || index < 0 || arr->len == 0 || arr->p == NULL) {
        return (InstPos){.line = 0, .colpos = 0, .length = 0, .is_virt = true};
    } else {
        return arr->p[index];
    }
}

bool make_changes_for_mod(Instruction *ins) {
    if (ins->len < 2) {
        return false;
    }
    ins->code[ins->len - 1] = OP_END_MOD;
    // ins->len--;
    return true;
}

/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_instruction_h
#define cpank_instruction_h
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "value.h"

typedef enum {
    OP_RETURN,
    OP_CONST,
    OP_NEG,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_NOT,
    OP_EQ,
    OP_GT,
    OP_GTE,
    OP_LT,
    OP_LTE,
    // OP_SHOW, -> Removed use show(...) built-in function
    OP_POP,
    OP_DEF_GLOB,
    OP_SET_GLOB,
    OP_GET_GLOB,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_JMP_IF_FALSE,
    OP_JMP,
    OP_LOOP,
    OP_CALL,
    OP_CLOSURE,
    OP_GET_UP,
    OP_SET_UP,
    OP_CLS_UP,
    OP_IMPORT_NONAME,
    OP_SET_MOD_PROP,
    OP_GET_MOD_PROP,
    OP_END_MOD,
    OP_ERR,
    OP_ARRAY,
    OP_HMAP,
    OP_ARR_INDEX,
    OP_SUBSCRIPT_ASSIGN,

} Op;

typedef struct {
    bool is_virt;
    uint64_t colpos;
    uint64_t line;
    uint64_t length;
} InstPos;

typedef struct {
    int len;
    int cap;
    InstPos *p;
} InstPosArray;

void init_iparr(InstPosArray *arr);
void free_iparr(PankVm *vm, InstPosArray *arr);
void add_iparr(PankVm *vm, InstPosArray *arr, InstPos pos);
InstPos fetch_iparr(InstPosArray *arr, int index);

typedef struct {
    int len;
    int cap;
    uint8_t *code;
    int *lines;
    Valarr consts;
    InstPosArray posarr;
} Instruction;

void init_instruction(Instruction *ins);
void write_ins(PankVm *vm, Instruction *ins, uint8_t bt, int line);
void free_ins(PankVm *vm, Instruction *ins);
int add_const(PankVm *vm, Instruction *ins, Value val);
bool make_changes_for_mod(Instruction *ins);

#ifdef __cplusplus
}
#endif
#endif

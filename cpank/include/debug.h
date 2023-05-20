/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_debug_h
#define cpank_debug_h
#ifdef __cplusplus
extern "C" {
#endif
#include <uchar.h>

#include "instruction.h"
void dissm_ins_chunk(Instruction *ins, const char32_t *name);
int dissm_ins(Instruction *ins, int offset);
const char *print_opcode(Op op);
#ifdef __cplusplus
}
#endif
#endif

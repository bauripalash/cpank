#include "include/debug.h"
#include "include/instruction.h"
#include "include/obj.h"
#include "include/value.h"
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>

// disassemble instructions set
void dissm_ins_chunk(Instruction *ins, const wchar_t *name) {
  wprintf(L"----> %ls <----\n", name);

  for (int off = 0; off < ins->len;) {
    off = dissm_ins(ins, off);
  }
}

// debug constant instruction
int const_ins(const char *name, Instruction *ins, int off) {
  uint8_t con = ins->code[off + 1];
  wprintf(L"%-16s %4d '", name, con);
  print_val(ins->consts.values[con]);
  wprintf(L"'\n");
  return off + 2;
}

// debug instruction with no operands
// such as OP_ADD , OP_SUB etc etc.
int simple_ins(const char *name, int offset) {
  wprintf(L"%s\n", name);
  return offset + 1;
}

// debug set/get type instructions
// OP_GET_LOCAL , OP_SET_LOCAL
int bt_ins(const char *name, Instruction *ins, int offset) {
  uint8_t slot = ins->code[offset + 1];
  wprintf(L"%-16s %4d\n", name, slot);
  return offset + 2;
}

// debug jump instructions
// OP_JMP_IF_FALSE , OP_LOOP
// OP_JMP
int jmp_ins(const char *name, int sign, Instruction *ins, int offset) {

  uint16_t jmp = (uint16_t)(ins->code[offset + 1] << 8);
  jmp |= ins->code[offset + 2];
  wprintf(L"%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jmp);
  return offset + 3;
}

// disassemble single instruction
int dissm_ins(Instruction *ins, int offset) {
  wprintf(L"%04d ", offset);
  if (offset > 0 && ins->lines[offset] == ins->lines[offset - 1]) {
    wprintf(L"   | ");
  } else {
    wprintf(L"%4d ", ins->lines[offset]);
  }
  uint8_t is = ins->code[offset];
  switch (is) {
  case OP_RETURN:
    return simple_ins("OP_RETURN", offset);
  case OP_NEG:
    return simple_ins("OP_NEG", offset);
  case OP_ADD:
    return simple_ins("OP_ADD", offset);
  case OP_SUB:
    return simple_ins("OP_SUB", offset);
  case OP_MUL:
    return simple_ins("OP_MUL", offset);
  case OP_DIV:
    return simple_ins("OP_DIV", offset);
  case OP_CONST:
    return const_ins("OP_CONST", ins, offset);
  case OP_NIL:
    return simple_ins("OP_NIL", offset);
  case OP_TRUE:
    return simple_ins("OP_TRUE", offset);
  case OP_FALSE:
    return simple_ins("OP_FALSE", offset);
  case OP_NOT:
    return simple_ins("OP_NOT", offset);
  case OP_EQ:
    return simple_ins("OP_EQ", offset);
  case OP_GT:
    return simple_ins("OP_GT", offset);
  case OP_LT:
    return simple_ins("OP_LT", offset);
  case OP_SHOW:
    return simple_ins("OP_SHOW", offset);
  case OP_POP:
    return simple_ins("OP_POP", offset);
  case OP_DEF_GLOB:
    return const_ins("OP_DEF_GLOB", ins, offset);
  case OP_GET_GLOB:
    return const_ins("OP_GET_GLOB", ins, offset);
  case OP_SET_GLOB:
    return const_ins("OP_SET_GLOB", ins, offset);
  case OP_GET_LOCAL:
    return bt_ins("OP_GET_LOCAL", ins, offset);
  case OP_SET_LOCAL:
    return bt_ins("OP_SET_LOCAL", ins, offset);
  case OP_CALL:
    return bt_ins("OP_CALL", ins, offset);
  case OP_JMP:
    return jmp_ins("OP_JMP", 1, ins, offset);
  case OP_JMP_IF_FALSE:
    return jmp_ins("OP_JMP_IF_FALSE", 1, ins, offset);
  case OP_LOOP:
    return jmp_ins("OP_LOOP", -1, ins, offset);
  case OP_GET_UP:
    return bt_ins("OP_GET_UP", ins, offset);
  case OP_SET_UP:
    return bt_ins("OP_SET_UP", ins, offset);
  case OP_CLS_UP:
    return simple_ins("OP_CLS_UPV", offset);
  case OP_CLOSURE:
    offset++;
    uint8_t con = ins->code[offset++];
    wprintf(L"%-16s %4d", "OP_CLOSURE", con);
    print_val(ins->consts.values[con]);
    wprintf(L"\n");

    ObjFunc *func = get_as_func(ins->consts.values[con]);

    // read up values after closure declaration
    for (int x = 0; x < func->up_count; x++) {
      int is_local = ins->code[offset++];
      int index = ins->code[offset++];
      wprintf(L"%04d  |   -> %s %d\n", offset - 2,
              is_local ? "local" : "upvalue", index);
    }

    return offset;
  default:
    printf("Unknown op %d\n", is);
    return offset + 1;
  }
}

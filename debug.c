#include "include/debug.h"
#include "include/instruction.h"
#include "include/value.h"
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>

void dissm_ins_chunk(Instruction *ins, const char *name) {
  wprintf(L"----> %s <----\n", name);

  for (int off = 0; off < ins->len;) {
    off = dissm_ins(ins, off);
  }
}
int const_ins(const char *name, Instruction *ins, int off) {
  //
  //	ins->code[off] => the offcode itself
  uint8_t con = ins->code[off + 1];
  wprintf(L"%-16s %4d ", name, con);
  print_val(ins->consts.values[con]);
  wprintf(L"\n");
  return off + 2;
}
int simple_ins(const char *name, int off) {
  wprintf(L"%s\n", name);
  return off + 1;
}
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
  default:
    printf("Unknown op %d\n", is);
    return offset + 1;
  }
}

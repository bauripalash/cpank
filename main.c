#include <locale.h>
#include <stdio.h>

#include "include/debug.h"
#include "include/instruction.h"
#include "include/runfile.h"
#include "include/vm.h"
int main() {
  setlocale(LC_CTYPE, "");
  boot_vm();
  // runcode("sample/sample.txt");
  Instruction ins;
  init_instruction(&ins);
  int con = add_const(&ins, 3.14);
  write_ins(&ins, OP_CONST, 10);
  write_ins(&ins, con, 10);
  write_ins(&ins, OP_RETURN, 10);
  dissm_ins_chunk(&ins, "<main>");
  interpret(&ins);
  // printf("%s\n" , ins.code);
  free_ins(&ins);
}

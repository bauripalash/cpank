#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "include/debug.h"
#include "include/instruction.h"
#include "include/runfile.h"
#include "include/vm.h"
int main() {
  setlocale(LC_CTYPE, "");
  boot_vm();
  Srcfile srcfile = read_file("sample/2.txt");
  wchar_t *s = (wchar_t *)malloc(sizeof(wchar_t) * srcfile.size);
  mbstowcs(s, srcfile.source, srcfile.size);
  interpret(s);
  // runcode("sample/sample.txt");
  // Instruction ins;
  // init_instruction(&ins);
  // int con = add_const(&ins, 3.14);
  // write_ins(&ins, OP_CONST, 10);
  // write_ins(&ins, con, 10);
  //  write_ins(&ins, OP_NEG, 10);
  // write_ins(&ins, OP_CONST, 10);
  // write_ins(&ins, add_const(&ins, 100), 10);
  // write_ins(&ins, OP_ADD, 100);
  // write_ins(&ins, OP_RETURN, 10);
  // dissm_ins_chunk(&ins, "<main>");
  // interpret(&ins);
  // printf("%s\n" , ins.code);
  // free_ins(&ins);
  free(srcfile.source);
  free(s);
}

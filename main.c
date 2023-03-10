#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "include/debug.h"
#include "include/instruction.h"
#include "include/runfile.h"
#include "include/vm.h"
int main() {
  int errcode = 0;
  setlocale(LC_CTYPE, "");
  boot_vm();
  Srcfile srcfile = read_file("sample/2.txt");
  wchar_t *s = (wchar_t *)malloc(sizeof(wchar_t) * srcfile.size);
  mbstowcs(s, srcfile.source, srcfile.size);
  IResult res = interpret(s);
  switch (res) {
  case INTRP_RUNTIME_ERR:
    wprintf(L"Runtime error occured!");
    errcode = 1;
    break;
    ;
  case INTRP_COMPILE_ERR:
    wprintf(L"Compiler error occured!");
    errcode = 1;
    break;
    ;
  case INTRP_OK:
    wprintf(L"OK");
    break;
  }
  wprintf(L"\n");
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
  free_vm(); // <<---- IMPORTANT
  free(s);
  free(srcfile.source);
  exit(errcode);
}

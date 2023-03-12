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
  return run_file("sample/fib.txt");
}

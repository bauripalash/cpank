#include "lexer.h"
#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
wchar_t *y = L"নাম";
wchar_t *x = L"(){};,-+/*>>=<<=\n\
             \"hello\"\
             ";
typedef struct {
  char *source;
  size_t size;
} Srcfile;

Srcfile read_file(char *fpath) {

  // setlocale(LC_CTYPE, "");
  FILE *fl = fopen(fpath, "rb");
  if (fl == NULL) {
    fprintf(stderr, "Failed to open file %s\n", fpath);
    exit(1);
  }
  fseek(fl, 0L, SEEK_END);
  size_t fsz = ftell(fl);
  rewind(fl);
  // wprintf(L"size -> %d\n", fsz);
  char *buff = (char *)malloc(fsz + 1);
  if (buff == NULL) {
    fprintf(stderr, "Not enough memory to read file '%s'\n", fpath);
    exit(1);
  }

  size_t byteread = fread(buff, sizeof(char), fsz, fl);

  // setlocale(LC_CTYPE, "");
  // wprintf(L"C-> %d\n", byteread);
  if (byteread < fsz) {
    fprintf(stderr, "Byteread -> Failed to read the file at '%s'\n", fpath);
    fclose(fl);
    free(buff);
    exit(1);
  }
  buff[byteread] = '\0';
  fclose(fl);
  Srcfile src;
  src.source = buff;
  src.size = fsz;
  return src;
}

void run(wchar_t *src) {

  setlocale(LC_CTYPE, "");
  boot_lexer(src);
  while (!is_eof()) {
    Token tk = get_tok();
    if (tk.type != T_ERR) {
      wprintf(L"T[%s] '%.*ls' \n", toktype_to_string(tk.type), tk.length,
              tk.start);
    } else {
      wprintf(L"E[] '%ls'\n", tk.start);
    }
  }
}

void runcode(char *fpath) {

  setlocale(LC_CTYPE, "");
  Srcfile src = read_file(fpath);
  wchar_t *s = (wchar_t *)malloc(sizeof(wchar_t) * src.size);
  // https://stackoverflow.com/a/13438980/7917825
  mbstowcs(s, src.source, src.size);
  // wprintf(L"READ -> %ls\n" , s);
  // printf("SRC-> %s\n", src);
  run(s);
  // wprintf(L"SOURCE_AFTER -> %ls\n\nSOURCE_ORIGINAL -> %s\n\n" , s ,
  // src.source);
  free(src.source);
  free(s);
}

int main() {

  // setlocale(LC_CTYPE, "en-US.UTF-8");

  runcode("./sample.txt");
}

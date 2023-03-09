#include <stddef.h>
#include <wchar.h>
typedef struct {

  char *source;
  size_t size;

} Srcfile;

Srcfile read_file(char *filepath);
void run(wchar_t *source);
void runcode(char *filepath);

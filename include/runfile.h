#include <stddef.h>
#include <wchar.h>
typedef struct {

  char *source;
  size_t size;

} Srcfile;

wchar_t *read_file(const char *filepath);
int run_file(const char *filepath);

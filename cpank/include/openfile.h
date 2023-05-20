/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_openfile_h
#define cpank_openfile_h
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <uchar.h>

// * char * source -> Source code content (you must free it)
// * size_t size -> size of the source file
typedef struct Srcfile {
    char *source;
    size_t size;
    int errcode;
} Srcfile;

// Widechar equivalent for Srcfile
// * wchar_t * source -> Source code content (you must free it)
// * size_t size -> size of the source file
typedef struct WSrcfile {
    char32_t *source;
    size_t size;
    int errcode;

} WSrcfile;

// Given a path (const char * path) return Srcfile
// see Srcfile
// you must free `Srcfile.source`
Srcfile read_file(const char *path);

// Widechar equivalent of Srcfile read_file(..)
// see `read_file` & `WSrcfile`
// you must free `WSrcfile.source`
WSrcfile wread_file(char32_t *path);

// Given `/foodir/bar/hello.txt` returns `hello`
// ** Unimplemented **
char32_t *get_name_from_path(const char *path);
#ifdef __cplusplus
}
#endif
#endif

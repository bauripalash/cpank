#ifndef cpank_openfile_h
#define cpank_openfile_h

#include <stddef.h>

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
    wchar_t *source;
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
WSrcfile wread_file(wchar_t *path);

// Given `/foodir/bar/hello.txt` returns `hello`
// ** Unimplemented **
wchar_t *get_name_from_path(const char *path);

#endif
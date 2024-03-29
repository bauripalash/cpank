/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "include/openfile.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

#include "include/common.h"
#include "include/utils.h"

#ifdef IS_WIN
 #include <windows.h>
#endif

Srcfile read_file(const char *path) {
    Srcfile result;

    result.source = NULL;
    result.size = 0;
    if (!does_file_exist(path)) {
        result.errcode = ERC_FILE_NOT_EXIST;
        return result;
    }
    result.errcode = 0;

#ifdef IS_WIN
    wchar_t *fname = (wchar_t *)malloc((strlen(path) + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, path, -1, fname, strlen(path) + 1);
    FILE *file = _wfopen(fname, L"rb, ccs=UTF-8");
#else
    FILE *file = fopen(path, "rb");
#endif

    if (file == NULL) {
        // fwprintf(stderr, L"Failed to open file %s\n", path);
        // cp_color_println('r' , L"Failed to open file %s" , path);
        result.errcode = ERC_FAIL_TO_OPEN;
        result.source = NULL;
        result.size = 0;
        fclose(file);
        return result;
    }
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    result.size = file_size + 1;

    char *temp_buff = (char *)malloc(file_size + 1);
    if (temp_buff == NULL) {
        result.errcode = ERC_NO_MEM;
        fclose(file);
        return result;
        // fwprintf(stderr, L"not enough memory %s\n", path);
        // exit(1);
    }
    size_t br = fread(temp_buff, sizeof(char), file_size, file);
    if (br < file_size) {
        result.errcode = ERC_NO_MEM;
        free(temp_buff);
        fclose(file);
        return result;
        // fwprintf(stderr, L"failed to read file %s\n", path);
        // exit(1);
    }
    temp_buff[br] = '\0';
    fclose(file);
    result.source = (char *)malloc(result.size);
    if (result.source == NULL) {
        free(temp_buff);
        result.errcode = ERC_NO_MEM;
        result.size = 0;
        return result;
    }
    memcpy(result.source, temp_buff, result.size);
    free(temp_buff);

    return result;
}

WSrcfile wread_file(char32_t *path) {
    WSrcfile result;
    result.source = NULL;
    result.size = 0;

    result.errcode = 0;
    // size_t path_size = sizeof(char32_t) * (strlen32(path) + 1);
    char *cpath =
        c_to_c(path, strlen32(path) + 1);  // malloc(sizeof(char) * path_size);

    if (cpath == NULL) {
        result.errcode = ERC_NO_MEM;
        return result;
    }

    // memset(cpath, 0, path_size);
    // wcstombs(cpath, path, wcslen(path));
    Srcfile raw = read_file(cpath);
    if (raw.errcode != 0) {
        result.errcode = raw.errcode;
        free(cpath);

        return result;
    }
    // size_t result_size = sizeof(wchar_t) * raw.size;
    // result.source = (wchar_t *)malloc(result_size);
    result.source = char_to_32(raw.source);
    if (result.source == NULL) {
        result.errcode = ERC_NO_MEM;
        free(cpath);
        return result;
    }

    // memset(result.source, 0, result_size);
    // mbstowcs(result.source, raw.source, raw.size + 1);
    free(raw.source);
    free(cpath);
    result.size = sizeof(result.source);
    return result;
}

// cppcheck-suppress unusedFunction
char32_t *get_name_from_path(const char *path) { return U""; }

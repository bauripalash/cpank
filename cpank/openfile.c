#include "include/openfile.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

#include "include/common.h"
#include "include/utils.h"

Srcfile read_file(const char *path) {
    Srcfile result;

    result.source = NULL;
    result.size = 0;
    if (!does_file_exist(path)) {
        result.errcode = ERC_FILE_NOT_EXIST;
        return result;
    }
    result.errcode = 0;
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        // fwprintf(stderr, L"Failed to open file %s\n", path);
        // cp_color_println('r' , L"Failed to open file %s" , path);
        result.errcode = ERC_FAIL_TO_OPEN;

        return result;
    }
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    result.size = file_size + 1;

    char *temp_buff = (char *)malloc(file_size + 1);
    if (temp_buff == NULL) {
        result.errcode = ERC_NO_MEM;
        return result;
        // fwprintf(stderr, L"not enough memory %s\n", path);
        // exit(1);
    }
    size_t br = fread(temp_buff, sizeof(char), file_size, file);
    if (br < file_size) {
        result.errcode = ERC_NO_MEM;
        free(temp_buff);
        return result;
        // fwprintf(stderr, L"failed to read file %s\n", path);
        // exit(1);
    }
    temp_buff[br] = '\0';
    fclose(file);
    result.source = (char *)malloc(result.size);
    memcpy(result.source, temp_buff, result.size);
    free(temp_buff);

    return result;
}

WSrcfile wread_file(char32_t *path) {
    WSrcfile result;
    result.source = NULL;
    result.size = 0;

    result.errcode = 0;
    // size_t path_size = sizeof(char32_t) * (strlen16(path) + 1);
    char *cpath =
        c_to_c(path, strlen16(path) + 1);  // malloc(sizeof(char) * path_size);

    if (cpath == NULL) {
        result.errcode = ERC_NO_MEM;
        return result;
    }

    // memset(cpath, 0, path_size);
    // wcstombs(cpath, path, wcslen(path));
    Srcfile raw = read_file(cpath);
    if (raw.errcode != 0) {
        result.errcode = raw.errcode;

        return result;
    }
    // size_t result_size = sizeof(wchar_t) * raw.size;
    // result.source = (wchar_t *)malloc(result_size);
    result.source = chto16(raw.source);
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

char32_t *get_name_from_path(const char *path) { return U""; }

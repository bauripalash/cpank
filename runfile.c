#include "include/runfile.h"

#include <locale.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "include/vm.h"

Srcfile read_file(const char *path) {
    Srcfile result;
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fwprintf(stderr, L"Failed to open file %s\n", path);
        exit(1);
    }
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    result.size = file_size + 1;

    char *temp_buff = (char *)malloc(file_size + 1);
    if (temp_buff == NULL) {
        fwprintf(stderr, L"not enough memory %s\n", path);
        exit(1);
    }
    size_t br = fread(temp_buff, sizeof(char), file_size, file);
    if (br < file_size) {
        fwprintf(stderr, L"failed to read file %s\n", path);
        exit(1);
    }
    temp_buff[br] = '\0';
    fclose(file);
    result.source = (char *)malloc(result.size);
    memcpy(result.source, temp_buff, result.size);
    // memcpy_s(result.source , result.size , temp_buff , result.size);
    free(temp_buff);

    return result;
}

int run_file(const char *filepath) {
    Srcfile raw = read_file(filepath);
    // wprintf(L"SOURCE -> %s", raw.source);
    int errcode = 0;
    wchar_t *src = (wchar_t *)malloc(sizeof(wchar_t) * raw.size);
    // wprintf(L"sizes: wchar_t -> %d | char -> %d\n" , sizeof(wchar_t) ,
    // setlocale(LC_CTYPE, "");
    mbstowcs(src, raw.source, raw.size + 1);

    boot_vm();

    // wprintf(L"SOURCE_W -> %ls", src);
    IResult res = interpret(src);

    switch (res) {
        case INTRP_RUNTIME_ERR:
            wprintf(L"Runtime error occured!\n");
            errcode = 1;
            break;
            ;
        case INTRP_COMPILE_ERR:
            wprintf(L"Compiler error occured!\n");
            errcode = 1;
            break;
            ;
        // case INTRP_OK:
        //   wprintf(L"OK\n");
        //   break;
        default: {
        }
    }

    // free(src);
    free_vm();

    free(src);

    free(raw.source);
    return errcode;
}

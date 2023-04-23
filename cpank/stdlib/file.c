#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <uchar.h>

#include "../include/helper/os.h"
#include "../include/openfile.h"
#include "../include/stdlib.h"
#include "../include/utils.h"

#define _NUM_TO_STR(x) #x
#define NUMTOSTR(x)    _NUM_TO_STR(x)

#define no_android(funcname)                                                   \
 if (get_os_code() == OS_ANDROID_CODE) {                                       \
  return make_error(vm,                                                        \
                    U"file function " funcname " do not work on android yet"); \
 }

bool check_is_file(char* path) {
#ifdef PANK_OS_WIN
    struct _stat pbuf;
    _stat(path, &pbuf);
    return ((pbuf.st_mode & _S_IFREG) != 0);
#else
    struct stat pbuf;
    stat(path, &pbuf);
    return S_ISREG(pbuf.st_mode);
#endif
}

bool check_is_dir(char* path) {
#ifdef PANK_OS_WIN
    struct _stat pbuf;
    _stat(path, &pbuf);
    return ((pbuf.st_mode & _S_IFDIR) != 0);
#else
    struct stat pbuf;
    stat(path, &pbuf);
    return S_ISDIR(pbuf.st_mode);
#endif
}

Value _file_exists(PankVm* vm, int argc, Value* args) {
    //    char argstr[100];
    //    sprintf(argstr, "%d", argc);
    no_android("exists(path)");
    check_argc_count("exists(path)", 1, argc);
    /*if (argc != 1) {

        return make_error(vm, U"exists(a) only takes single argument");
    }*/
    if (!is_str_obj(args[0])) {
        return make_error(vm, U"argument to exists(a) must be a string");
    }

    ObjString* str = (ObjString*)get_as_obj(args[0]);
    char* fp = c_to_c(str->chars, str->len);

    if (does_file_exist(fp)) {
        free(fp);
        return make_bool(true);
    } else {
        free(fp);
        return make_bool(false);
    }
}

Value _file_read_file_as_string(PankVm* vm, int argc, Value* args) {
    no_android("readfile(path)");
    check_argc_count("readile(path)", 1, argc);

    if (!is_str_obj(args[0])) {
        return make_error(
            vm, U"argument to readfile(path <- string) must be a string");
    }
    ObjString* str = (ObjString*)get_as_obj(args[0]);

    WSrcfile fws = wread_file(str->chars);

    if (fws.errcode != 0) {
        return make_str(vm, U"");
    }

    Value result = make_str(vm, fws.source);
    free(fws.source);
    return result;
}

Value _file_is_file(PankVm* vm, int argc, Value* args) {
    no_android("isfile(path)");
    check_argc_count("isfile(path)", 1, argc);

    if (!is_str_obj(args[0])) {
        return make_error(
            vm, U"argument to isfile(path <- string) must be a string");
    }

    ObjString* wpath = (ObjString*)get_as_obj(args[0]);
    char* path = c_to_c(wpath->chars, wpath->len);

    bool result = check_is_file(path);
    free(path);
    return make_bool(result);
}

Value _file_is_dir(PankVm* vm, int argc, Value* args) {
    no_android("isdir(path)");
    check_argc_count("isdir(path)", 1, argc);

    ObjString* wpath = (ObjString*)get_as_obj(args[0]);
    char* path = c_to_c(wpath->chars, wpath->len);

    bool result = check_is_dir(path);
    free(path);
    return make_bool(result);
}

Value _file_create_empty_file(PankVm* vm, int argc, Value* args) {
    no_android("create_empty(path)");
    check_argc_count("create_empty(path)", 1, argc);
    if (!is_str_obj(args[0])) {
        return make_error(
            vm, U"argument to create_empty(path <- string) must be a string");
    }

    ObjString* wpath = (ObjString*)get_as_obj(args[0]);
    char* path = c_to_c(wpath->chars, wpath->len);
    bool result = true;
    FILE* fp;
    fp = fopen(path, "w");
    if (fp == NULL) {
        result = false;
    } else {
        fclose(fp);
    }
    free(path);
    return make_bool(result);
}

Value _file_rename(PankVm* vm, int argc, Value* args) {
    no_android("rename(old , new)");
    check_argc_count("rename(old , new)", 2, argc);

    if (!is_str_obj(args[0]) || !is_str_obj(args[1])) {
        return make_error(vm,
                          U"argument to rename(old <- string , new <- string) "
                          U"must be a string");
    }

    ObjString* wpath_old = (ObjString*)get_as_obj(args[0]);
    char* old_path = c_to_c(wpath_old->chars, wpath_old->len);
    ObjString* wpath_new = (ObjString*)get_as_obj(args[1]);
    char* new_path = c_to_c(wpath_new->chars, wpath_new->len);
    bool result = true;
    if (rename(old_path, new_path) != 0) {
        result = false;
    }
    free(old_path);
    free(new_path);
    return make_bool(result);
}

Value _file_delete(PankVm* vm, int argc, Value* args) {
    no_android("delete(path)");
    check_argc_count("delete(path)", 1, argc);

    if (!is_str_obj(args[0])) {
        return make_error(
            vm, U"argument to delete(path <- string) must be a string");
    }

    ObjString* wpath = (ObjString*)get_as_obj(args[0]);
    char* path = c_to_c(wpath->chars, wpath->len);
    bool result = true;
    if (remove(path) != 0) {
        result = false;
    }
    free(path);
    return make_bool(result);
}

// 0 -> All Okay
// 1 -> Failed to open file
// 2 -> Failed to write file
int write_to_file(Value vpath, Value vdata, bool append) {
    ObjString* raw_path = get_as_string(vpath);
    ObjString* raw_data = get_as_string(vdata);

    char* path = c_to_c(raw_path->chars, raw_path->len);
    char* data = c_to_c(raw_data->chars, raw_data->len);

    FILE* fp;
    if (append) {
        fp = fopen(path, "ab");
    } else {
        fp = fopen(path, "wb");
    }
    if (fp == NULL) {
        free(path);
        free(data);
        return 1;
    }

    size_t data_len = strlen(data);
    size_t br = fwrite(data, sizeof(char), data_len, fp);
    if (br < data_len) {
        free(path);
        free(data);
        fclose(fp);
        return 2;
    }

    free(path);
    free(data);
    fclose(fp);
    return 0;
}

Value _file_write_string(PankVm* vm, int argc, Value* args) {
    no_android("write(path, data)");
    check_argc_count("write(path , data)", 2, argc);

    if (!is_str_obj(args[0])) {
        return make_error(vm,
                          U"first argument (file path) to write(path, data) "
                          U"must be a string");
    }

    if (!is_str_obj(args[1])) {
        return make_error(vm,
                          U"second argument (data to write) to write(path, "
                          U"data) must be a string");
    }

    int res = write_to_file(args[0], args[1], false);
    if (res == 1) {
        return make_error(vm, U"failed to open/create file");
    } else if (res == 2) {
        return make_error(vm, U"failed to write data to file");
    } else {
        return make_bool(true);
    }
}

Value _file_append_string(PankVm* vm, int argc, Value* args) {
    no_android("append(path, data)");
    check_argc_count("append(path , data)", 2, argc);

    if (!is_str_obj(args[0])) {
        return make_error(vm,
                          U"first argument (file path) to write(path, data) "
                          U"must be a string");
    }

    if (!is_str_obj(args[1])) {
        return make_error(vm,
                          U"second argument (data to write) to write(path, "
                          U"data) must be a string");
    }

    int res = write_to_file(args[0], args[1], true);
    if (res == 1) {
        return make_error(vm, U"failed to open file");
    } else if (res == 2) {
        return make_error(vm, U"failed to write data to file");
    } else {
        return make_bool(true);
    }
}

void push_stdlib_file(PankVm* vm) {
    SL sls[] = {
        msl(U"exists", _file_exists),
        msl(U"readfile", _file_read_file_as_string),
        msl(U"isfile", _file_is_file),
        msl(U"isdir", _file_is_dir),
        msl(U"create_empty", _file_create_empty_file),
        msl(U"rename", _file_rename),
        msl(U"delete", _file_delete),
        msl(U"write", _file_write_string),
        msl(U"append", _file_append_string),
    };

    _push_stdlib(vm, U"file", sls, 9);
}

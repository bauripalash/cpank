#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/helper/os.h"
#include "../include/openfile.h"
#include "../include/stdlib.h"
#include "../include/utils.h"

bool check_is_file(char* path) {
#ifdef PANK_OS_WINDOWS
    struct _stat pbuf;
 #define S_ISREG _S_IFREG

    _stat(path, &pbuf);
#else
    struct stat pbuf;
    stat(path, &pbuf);
#endif
    return S_ISREG(pbuf.st_mode);
}

bool check_is_dir(char* path) {
#ifdef PANK_OS_WINDOWS
    struct _stat pbuf;
 #define S_ISDIR _S_IFDIR

    _stat(path, &pbuf);
#else
    struct stat pbuf;
    stat(path, &pbuf);
#endif
    return S_ISDIR(pbuf.st_mode);
}

Value _file_exists(PankVm* vm, int argc, Value* args) {
    if (get_os_code() == OS_ANDROID_CODE) {
        return make_error(vm, U"exists(path) does not work on android yet");
    }
    if (argc != 1) {
        return make_error(vm, U"exists(a) only takes single argument");
    }
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
    if (get_os_code() == OS_ANDROID_CODE) {
        return make_error(vm, U"readfile(path) does not work on android yet");
    }
    if (argc != 1) {
        return make_error(vm, U"readfile(path) takes only single argument");
    }

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
    if (get_os_code() == OS_ANDROID_CODE) {
        return make_error(vm, U"isfile(path) does not work on android yet");
    }
    if (argc != 1) {
        return make_error(vm, U"isfile(path) takes only single argument");
    }
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
    if (get_os_code() == OS_ANDROID_CODE) {
        return make_error(vm, U"isdir(path) does not work on android yet");
    }
    if (argc != 1) {
        return make_error(vm, U"isdir(path) takes only single argument");
    }
    if (!is_str_obj(args[0])) {
        return make_error(
            vm, U"argument to isdir(path <- string) must be a string");
    }

    ObjString* wpath = (ObjString*)get_as_obj(args[0]);
    char* path = c_to_c(wpath->chars, wpath->len);

    bool result = check_is_dir(path);
    free(path);
    return make_bool(result);
}

Value _file_create_empty_file(PankVm* vm, int argc, Value* args) {
    if (get_os_code() == OS_ANDROID_CODE) {
        return make_error(vm,
                          U"create_empty(path) does not work on android yet");
    }
    if (argc != 1) {
        return make_error(vm, U"create_empty(path) takes only single argument");
    }
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
    }
    fclose(fp);
    free(path);
    return make_bool(result);
}

Value _file_rename(PankVm* vm, int argc, Value* args) {
    if (get_os_code() == OS_ANDROID_CODE) {
        return make_error(vm, U"rename(path) does not work on android yet");
    }
    if (argc != 2) {
        return make_error(vm,
                          U"rename(oldname , newname) takes only 2 argument");
    }
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
    if (get_os_code() == OS_ANDROID_CODE) {
        return make_error(vm, U"delete(path) does not work on android yet");
    }
    if (argc != 1) {
        return make_error(vm, U"delete(path) takes only single argument");
    }
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

void push_stdlib_file(PankVm* vm) {
    SL sls[] = {
        msl(U"exists", _file_exists),
        msl(U"readfile", _file_read_file_as_string),
        msl(U"isfile", _file_is_file),
        msl(U"isdir", _file_is_dir),
        msl(U"create_empty", _file_create_empty_file),
        msl(U"rename", _file_rename),
        msl(U"delete", _file_delete),
    };

    _push_stdlib(vm, U"file", sls, 7);
}

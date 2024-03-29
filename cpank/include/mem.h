/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_mem_h
#define cpank_mem_h
#ifdef __cplusplus
extern "C" {
#endif
#include "common.h"
#include "obj.h"
#include "value.h"

#define ALLOC(vm, type, count) \
 (type *)rallc(vm, NULL, 0, sizeof(type) * (count))

#define GROW_CAP(cap) ((cap) < 8 ? 8 : (cap)*2)

#define GROW_ARR(vm, type, ptr, oc, nc) \
 (type *)rallc(vm, ptr, sizeof(type) * (oc), sizeof(type) * (nc))

#define FREE_ARR(vm, type, ptr, oc) rallc(vm, ptr, sizeof(type) * (oc), 0)
#define FREE(vm, type, ptr)         rallc(vm, ptr, sizeof(type), 0)

// It's just GROW_ARR with un-confusing name
#define SHRINK(vm, prev, type, oldc, newc) \
 (type *)rallc(vm, prev, sizeof(type) * (oldc), sizeof(type) * (newc))

typedef struct GcConfig {
    bool is_paused;
} GcConfig;

extern GcConfig gcon;

void *rallc(PankVm *vm, void *ptr, size_t osize, size_t nsize);
void collect_garbage(PankVm *vm);
void free_objs(PankVm *vm);
void mark_val(PankVm *vm, Value val);
void mark_obj(PankVm *vm, Obj *obj);
void free_single_obj(PankVm *vm, Obj *obj);
void free_value(PankVm *vm, Value val);
#ifdef __cplusplus
}
#endif
#endif

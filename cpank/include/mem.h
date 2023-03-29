#ifndef cpank_mem_h
#define cpank_mem_h

#include "common.h"
#include "value.h"

#define ALLOC(type, count) (type *)rallc(NULL, 0, sizeof(type) * (count))

#define GROW_CAP(cap) ((cap) < 8 ? 8 : (cap)*2)

#define GROW_ARR(type, ptr, oc, nc) \
    (type *)rallc(ptr, sizeof(type) * (oc), sizeof(type) * (nc))

#define FREE_ARR(type, ptr, oc) rallc(ptr, sizeof(type) * (oc), 0)
#define FREE(type, ptr) rallc(ptr, sizeof(type), 0)

typedef struct GcConfig {
    bool is_paused;
} GcConfig;

extern GcConfig gcon;

void *rallc(void *ptr, size_t osize, size_t nsize);
void collect_garbage(void);
void free_objs(void);
void mark_val(Value val);
void mark_obj(Obj *obj);
void free_single_obj(Obj *obj);

#endif

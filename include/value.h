#ifndef cpank_value_h
#define cpank_value_h

#include "common.h"
typedef double Value;

typedef struct {
  int cap;
  int len;
  Value *values;
} Valarr;

void init_valarr(Valarr *array);
void write_valarr(Valarr *array, Value value);
void free_valarr(Valarr *array);
void print_val(Value val);

#endif

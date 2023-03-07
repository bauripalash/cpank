#include <stdint.h>

typedef enum {
	OP_RETURN,
} Op;


typedef struct {
	int len;
	int cap;
	uint8_t* code; 
	
} Instruction;

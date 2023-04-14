package gopkg

import (
	_ "cpank/cpank"
	_ "cpank/cpank/stdlib"
	"unsafe"
)

// #cgo LDFLAGS:-lm
// #include "../cpank/include/api.h"
// #include <stdlib.h>
import "C"

func DoParse(src string) string{
	csrc := C.CString(src)
	defer C.free(unsafe.Pointer(csrc))
	result := C.run_code(csrc)	
	return C.GoString(result)
}

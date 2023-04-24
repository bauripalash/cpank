package gopkg

import (
	_ "go.cs.palashbauri.in/pankti/cpank"
	_ "go.cs.palashbauri.in/pankti/cpank/stdlib"
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

package androidapi

// #cgo CFLAGS: -g -Wall
// #cgo LDFLAGS: -lm
// #include "androidapi.h"
// #include <stdlib.h>
import "C"
import "unsafe"

//export DoParse
func DoParse(src string) string{
	csrc := C.CString(src)
	defer C.free(unsafe.Pointer(csrc))
	raw_result := C.do_parse_w(csrc)
	defer C.free(unsafe.Pointer(raw_result))
	result := C.GoString(raw_result)
	return result
}

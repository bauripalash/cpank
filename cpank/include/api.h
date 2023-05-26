#ifndef cpank_api_h
#define cpank_api_h
#ifdef __cplusplus
extern "C" {
#endif

// returned result must be freed.
// each call to `run_code` creates and frees the VM.
// which makes it very unoptimized.
// we need to make it little more optimized.
char* run_code(char* source);

#ifdef __cplusplus
}
#endif
#endif

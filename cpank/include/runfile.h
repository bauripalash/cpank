/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_runfile_h
#define cpank_runfile_h
#ifdef __cplusplus
extern "C" {
#endif

int run_stdin();
int run_file(const char *filepath);
#ifdef __cplusplus
}
#endif
#endif

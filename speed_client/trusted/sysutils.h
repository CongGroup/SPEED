#ifndef SYSUTILS_H
#define SYSUTILS_H

#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string>
#include "../../common/data_type.h"

#if defined(__cplusplus)
extern "C" {
#endif

void eprintf(const char *fmt, ...);

void get_time(hrtime *time);

void load_text_file(const char *filename, char **textfile, int *filesize);

void write_text_file(const char *filename, const char *textfile, int filesize);

std::string loadFiletoString(const char* path);
std::string loadArraytoString(void* src, int count, int eleSize);
std::string hashString(const std::string src);

// split str with chs, return first one, and modify dummy point to rest str; if none chs, *dummy is null.
char* strsep(char** dummy, const char* chs);

#if defined(__cplusplus)
}
#endif

#endif /* !_ENCLAVE_H_ */

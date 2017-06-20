#ifndef SYSUTILS_H
#define SYSUTILS_H

#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "../../common/data_type.h"

#if defined(__cplusplus)
extern "C" {
#endif

void eprintf(const char *fmt, ...);

void get_time(hrtime *time);

#if defined(__cplusplus)
}
#endif

#endif /* !_ENCLAVE_H_ */

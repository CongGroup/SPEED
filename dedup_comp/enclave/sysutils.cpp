#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */

#include "sysutils.h"
#include "Enclave_t.h"

void eprintf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}

void get_time(hrtime *time)
{
    ocall_get_time(&time->second, &time->nanosecond);
}
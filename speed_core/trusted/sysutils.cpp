#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */

#include "sysutils.h"
#include "Enclave_t.h"
#include "string.h"

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

void load_text_file(const char *filename,
                    char **textfile, int *filesize)
{
    static char* buffer = new char[FILE_BUFFER_SIZE];
	char* out_buffer;
    ocall_load_text_file(filename, &out_buffer, FILE_BUFFER_SIZE, filesize);

	memcpy(buffer, out_buffer, *filesize);

    if (*filesize == 0)
        abort();
    *textfile = buffer;
}

void write_text_file(const char *filename,char *textfile, int filesize)
{
	ocall_write_text_file(filename, textfile, filesize);
}

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

// the buffer is outside of enclave
void load_text_file(const char *filename, char **textfile, int *filesize)
{
	int size = 0;
	ocall_file_size(&size, filename);
	char* buffer;// = new char[size];
	//eprintf("new %p\n", buffer);
	//memset(buffer, 0, size);
    ocall_load_text_file(filename, &buffer, size, filesize);
	if (size < *filesize)
	{
		eprintf("enclave load text size error, %d, %d.\n", size, *filesize);
	}
    *textfile = buffer;
}

void write_text_file(const char *filename, const char *textfile, int filesize)
{
	ocall_write_text_file(filename, textfile, filesize);
}

char* strsep(char** dummy, const char* chs)
{
	char* beg = *dummy;

	char* str = *dummy;
	if (str == 0 || (*str) == 0)
	{
		return 0;
	}

	while (*str)
	{
		if (!((*str) >= 'a' && (*str) <= 'z' || (*str) >= 'A' && (*str) <= 'Z' || (*str) >= '0' && (*str) <= '9' || (*str) == '\0'))
		{
			(*str) = ' ';
		}

		for (const char* ch = chs; *ch != 0; ch++)
		{
			if (*str == *ch)
			{
				*str = 0;
				break;
			}
		}

		if (*str)
		{
			str++;
		}
	}
	*dummy = str + 1;
	return beg;
}

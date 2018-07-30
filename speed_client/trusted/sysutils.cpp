#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */

#include "sysutils.h"
#include "Enclave_t.h"
#include "string.h"
#include "crypto.h"

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

std::string loadFiletoString(const char * path)
{
	char *textfile;
	int filesize;
	load_text_file(path, &textfile, &filesize);
	return std::string(textfile, filesize);
}

std::string loadArraytoString(void * src, int count, int eleSize)
{
	std::string res;
	char* pointer = (char*)src;
	for (int i = 0; i < count; i++)
	{
		res.append(pointer, eleSize);
		pointer += eleSize;
	}
	return res;
}

std::string hashString(const std::string src)
{
	byte hashr[HASH_SIZE] = { 0 };
	::hash((byte*)src.c_str(), src.size(), hashr); 
	return std::string((char*)hashr, HASH_SIZE);
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
				*dummy = str + 1;
				return beg;
			}
		}

		if (*str)
		{
			str++;
		}
	}
	
	*dummy = str;
	return beg;
}

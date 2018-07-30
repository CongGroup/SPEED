#include "mr_bow.h"
#include "Enclave_t.h"
#include "sysutils.h"
#include <stdio.h>
#include "FunctionDB.h"

#include "dedupTool.h"

const int buffer_size = 512;

extern bool dedup_switch;

extern char* dic5000[DIC_SIZE];

static char* bow2str(const int* bow5000)
{
	char* res =(char*) malloc(10240);
	if (!res)
	{
		eprintf("malloc failed in bow2str.\n");
	}
	char* pch = res;
	char buffer[8];
	for (int i = 0; i < DIC_SIZE; i++)
	{
		snprintf(buffer,8,"%d,", bow5000[i]);
		memcpy(pch, buffer, strlen(buffer));
		pch += strlen(buffer);
	}
	*(pch - 1) = 0;
	if (pch - res > 10240)
	{
		eprintf("Error: bow2str size too small, need %d.\n", pch - res);
	}
	return res;
}

static int str2num(const char* str)
{
	const char* pch = str;
	int num = 0;
	while ((*pch) >= '0' && (*pch) <= '9')
	{
		num = num * 10 + (*pch) - '0';
		pch += 1;
	}
	return num;
}

static int numlen(int n)
{
	int len = 1;
	while ((n /= 10) != 0)
		len += 1;
	return len;
}

static void str2bow(int* bow, const char* str)
{
	const char* pch = str;
	int value;
	for (int i = 0; i < DIC_SIZE; i++)
	{
		bow[i] = str2num(pch);
		pch += numlen(bow[i]) + 1;
		// just debug
		if (*(pch - 1) != ','&& *(pch - 1) != '\0')
		{
			eprintf("Error format in str2bow %s\n", str);
			abort();
		}
	}

}

static void bowaddequal(int* dst, const int * src)
{
	for (int i = 0; i < DIC_SIZE; i++)
	{
		dst[i] += src[i];
	}
}

// split buffer by delim and lookup with dic get keyword`s count to block_bow
void bow_map_word_without_dedup(char* buffer, const char* delim, int* block_bow)
{
	char *token, *dummy = buffer;
	while ((token = strsep(&dummy, delim)) != NULL)
	{
		if (strlen(token) <= 0)
		{
			continue;
		}
		for (int i = 0; i < DIC_SIZE; i++)
		{
			if (strcmp(token, dic5000[i]) == 0)
			{
				block_bow[i] += 1;
				//break;
			}
		}
	}
}

void bow_map_word_with_dedup(char* buffer, const char* delim, int* block_bow)
{
	DEDUP_FUNCTION_INIT;

	byte* pb;
	input_with_r_buffer_size = buffer_size + HASH_SIZE;
	input_with_r_buffer = new byte[input_with_r_buffer_size];
	memset(input_with_r_buffer, 0, input_with_r_buffer_size);

	output_buffer_size = DIC_SIZE*sizeof(int);
	output_buffer = new byte[output_buffer_size];
	memset(output_buffer, 0, output_buffer_size);

	pb = input_with_r_buffer;
	memcpy(pb, buffer, buffer_size);

	DEDUP_FUNCTION_QUERY;

	bow_map_word_without_dedup(buffer, delim, block_bow);
	output_true_size = output_buffer_size;
	pb = output_buffer;
	memcpy(pb, block_bow, output_true_size);

	DEDUP_FUNCTION_UPDATE;
	if (doDedup) 
	{
		pb = output_buffer;
		memcpy(block_bow, pb, output_buffer_size);
	}
	delete[] input_with_r_buffer;
	delete[] output_buffer;
}

static std::string computStringBow(const string& src)
{
	int block_bow[DIC_SIZE];
	const char* delim = " \t\r\n!@#$%^&*()-=_+[]{};':,./<>?|`~";

	bow_map_word_without_dedup((char*)src.c_str(), delim, block_bow);

	return  loadArraytoString(block_bow, DIC_SIZE, sizeof(int));
}

static std::string computStringBow_dedup(const string& src)
{
	std::string returnValue;

	std::string hash = computeStringHash(src);

	bool exist = queryByHash(hash);

	if (exist)
	{
		returnValue = getResultByHash(hash);
	}
	else
	{
		returnValue = computStringBow(src);
		putResultByHash(hash, returnValue);
	}

	return returnValue;
}

void bow_map(char * file_name)
{
	int fp;
	ocall_open(&fp, file_name, 0);

	
	char buffer[buffer_size] = { 0 };
	int size = 0;

	int word_frequency[DIC_SIZE] = { 0 };
	int block_bow[DIC_SIZE];

	const char* delim = " \t\r\n!@#$%^&*()-=_+[]{};':,./<>?|`~";

	while (1)
	{
		ocall_read(&size, fp, buffer, buffer_size - 1);
		if (size <= 0)
			break;

		memset(block_bow, 0 ,sizeof(block_bow));

		if (dedup_switch)
		{
			bow_map_word_with_dedup(buffer, delim, block_bow);
		}
		else
		{
			bow_map_word_without_dedup(buffer, delim, block_bow);
		}

		
		MR_Emit(file_name, bow2str(block_bow));
	}
	//MR_Emit(file_name, bow2str(word_frequency));

	
	ocall_close(fp);
}

void bow_reduce(char * key, Getter get_next, int partition_number)
{
	char *value;
	int bow[DIC_SIZE] = { 0 };
	int sub_bow[DIC_SIZE];
	while ((value = get_next(key, partition_number)) != NULL)
	{
		str2bow(sub_bow, value);
		bowaddequal(bow, sub_bow);
		//count++;
	}

	int wordcount = 0;
	for (int i = 0; i < DIC_SIZE; i++)
	{
		if (bow[i] != 0)
		{
			++wordcount;
		}
	}
	
	
	eprintf("%s have %d key words.\n", key, wordcount);
}

int bow_run(int file_count, char ** file_paths)
{
	int mapper_count = 4;
	int reducer_count = 2;

	eprintf("Use %d mapper and %d reducer for bow task.\n", mapper_count, reducer_count);

	MR_Run(file_count, file_paths, bow_map, mapper_count, bow_reduce, reducer_count, MR_DefaultHashPartition);

	
	return 0;
}
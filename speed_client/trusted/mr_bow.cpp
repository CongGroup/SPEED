#include "mr_bow.h"
#include "Enclave_t.h"
#include "sysutils.h"
#include <stdio.h>
#include "FunctionDB.h"
#include <string>
#include <map>

#include "Deduplicable.h"

#include "dedupTool.h"

using namespace std;

const int buffer_size = 512;

const int str_bow_max_size = 4096;

static map<string, int> wordIndex;

extern bool dedup_switch;

extern char* dic5000[DIC_SIZE];

static char* bow2str(const int* bow5000)
{
	char* res =(char*) malloc(str_bow_max_size);
	if (!res)
	{
		eprintf("malloc failed in bow2str.\n");
	}
	char* pch = res;
	char buffer[12];
	for (int i = 0; i < DIC_SIZE; i++)
	{
		if (bow5000[i])
		{
			snprintf(buffer, 8, "%d:%d,", i, bow5000[i]);
			memcpy(pch, buffer, strlen(buffer));
			pch += strlen(buffer);
		}
	}
	// remove tail ","
	*(pch - 1) = 0;
	if (pch - res > str_bow_max_size)
	{
		eprintf("Error: bow2str size too small, need %d.\n", pch - res);
	}
	return res;
}

static int str2distance(const char* str);

static string bow2string(const int* bow5000)
{
	string res;
	res.reserve(1024);
	char buffer[16];
	for (int i = 0; i < DIC_SIZE; i++)
	{
		if (bow5000[i])
		{
			snprintf(buffer, 16, "%d:%d,", i, bow5000[i]);
			res.append(buffer);
		}
	}
	if (!res.size())
	{
		res.assign("0:0,");
	}
	// remove tail ","
	if(res[res.size()-1] == ',')
		res.erase(res.end() - 1);
	
	return std::move(res);
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
	int index,value;
	memset(bow, 0, sizeof(int)*DIC_SIZE);

	for (int i = 0; i < DIC_SIZE; i++)
	{
		index = str2num(pch);
		pch += numlen(index) + 1;
		// just debug
		//if (*(pch - 1) != ','&& *(pch - 1) != ':'&& *(pch - 1) != '\0')
		//{
		//	eprintf("Error format in str2bow %s\n", str);
		//	abort();
		//}
		value = str2num(pch);
		pch += numlen(value) + 1;
		// just debug
		//if (*(pch - 1) != ','&& *(pch - 1) != ':'&& *(pch - 1) != '\0')
		//{
		//	eprintf("Error format in str2bow %s\n", str);
		//	abort();
		//}
		bow[index] = value;

		if (*(pch - 1) == '\0')
		{
			break;
		}
	}
}

static int str2distance(const char* str)
{
	const char* pch = str;
	int index, value;
	int res = 0;

	for (int i = 0; i < DIC_SIZE; i++)
	{
		index = str2num(pch);
		// just debug
		//if (*(pch+ numlen(index)) != ':')
		//{
		//	eprintf("Error format1 in str2bow %s\n", str);
		//	abort();
		//}
		pch += numlen(index) + 1;

		value = str2num(pch);
		// just debug
		//if (*(pch + numlen(value)) != ',' && *(pch + numlen(value)) != '\0')
		//{
		//	eprintf("Error format2 in str2bow %s\n", str);
		//	abort();
		//}
		pch += numlen(value) + 1;

		res += value * value;

		if (*(pch - 1) == '\0')
		{
			break;
		}
	}

	return res;
}

static void bowaddequal(int* dst, const int * src)
{
	for (int i = 0; i < DIC_SIZE; i++)
	{
		dst[i] += src[i];
	}
}

// split buffer by delim and lookup with dic get keyword`s count to block_bow
static string bow_mapper(const string& str, const string& delim)
{
	int bow[DIC_SIZE];
	memset(bow, 0, sizeof(bow));
	char *token, *dummy = (char*)str.data();

	while ((token = strsep(&dummy, delim.data())) != NULL)
	{
		if (strlen(token) <= 0)
		{
			continue;
		}
		else if (wordIndex.find(token) != wordIndex.end())
		{
			bow[wordIndex[token]] += 1;
		}
		
		//for (int i = 0; i < DIC_SIZE; i++)
		//{
		//	if (strcmp(token, dic5000[i]) == 0)
		//		break;
		//}
	}
	return bow2string(bow);
}

int max_res_size = 0;

void bow_map(char * file_name)
{
	int fp;
	eprintf("read file %s.\n", file_name);
	ocall_open(&fp, file_name, 0);

	int endline = 0;
	
	char buffer[buffer_size] = { 0 };
	int size = 0;

	int word_frequency[DIC_SIZE] = { 0 };
	//int block_bow[DIC_SIZE];

	string delim(" \t\r\n");
	while (1)
	{
		//read line
		string str;
		endline = 0;
		while (!endline)
		{
			if (size == 0)
			{
				memset(buffer, 0, buffer_size);
				ocall_read(&size, fp, buffer, buffer_size - 1);
			}

			if (size <= 0)
			{
				endline = 1;
			}
			else
			{
				char* pos = strstr(buffer, "\n");
				if (!pos)
				{
					str.append(buffer, size);
					size = 0;
				}
				else
				{
					str.append(buffer, pos - buffer);
					pos += 1;
					size -= (pos - buffer);
	
					memmove(buffer, pos, size);
					memset(buffer + size, 0, buffer_size - size);
					endline = 1;
				}
			}
		}
		if (str.size() == 0)
			break;

		//for debug
		//eprintf("one line context is %s.\n", str.c_str());

		int sp = str.find('\t');
		if (sp == string::npos)
		{
			eprintf("can`t find /t.\n", str.c_str());
		}
		string key(str.c_str(), sp);
		str.erase(0, sp + 1);


		//eprintf("key is %s.\n", key.c_str());
		//eprintf("value is %s.\n", str.c_str());

		string res;
		if (dedup_switch)
		{
			Deduplicable<string, const string&, const string&> dedup_func("sgx_mapreduce", "0.1.0", "bow_mapper", bow_mapper);
			dedup_func.get_output_minimum_size(65535);
			res = dedup_func(str, delim);
		}
		else
		{
			res = bow_mapper(str, delim);
		}
		//block_bow = (int(*)[DIC_SIZE])res.data();

		if (res.size() > max_res_size)
		{
		//	eprintf("new max rew %s.\n", res.data());
			max_res_size = res.size();
		}

		char* heapstr = (char*)malloc(res.size() + 1);
		memcpy(heapstr, res.data(), res.size() + 1);
		MR_Emit(key.c_str(), heapstr);
	}
	//MR_Emit(file_name, bow2str(word_frequency));

	ocall_close(fp);
}

void bow_reduce(char * key, Getter get_next, int partition_number)
{
	const char *value;
	//int bow[DIC_SIZE] = { 0 };
	//int sub_bow[DIC_SIZE];
	int distance = 0;
	while ((value = get_next(key, partition_number)) != NULL)
	{
		//str2bow(sub_bow, value);
		//bowaddequal(bow, sub_bow);
		distance += str2distance(value);
	}

	//int wordcount = 0;
	//for (int i = 0; i < DIC_SIZE; i++)
	//{
	//	if (bow[i] != 0)
	//	{
	//		++wordcount;
	//	}
	//}
	//eprintf("%s distance is %d.\n", key, distance);
}

int bow_run(int file_count, char ** file_paths)
{
	int mapper_count = 4;
	int reducer_count = 2;


	eprintf("Use %d mapper and %d reducer for bow task.\n", mapper_count, reducer_count);

	// init
	if (!wordIndex.size())
	{
		for (int i = 0; i < DIC_SIZE; i++)
		{
			wordIndex[dic5000[i]] = i;
		}
	}

	// run
	MR_Run(file_count, file_paths, bow_map, mapper_count, bow_reduce, reducer_count, MR_DefaultHashPartition);

	//eprintf("max res size is %d.\n", max_res_size);
	return 0;
}
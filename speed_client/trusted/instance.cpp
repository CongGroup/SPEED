#include "Enclave_t.h"

#include <string>

#include "compression.h"
#include "dedup_service.h"
#include "function.h"
#include "pattern_matching.h"
#include "sysutils.h"
#include "word_count.h"
#include "sift_function.h"
#include "string.h"
#include "mr_wordcount.h"
#include "mr_bow.h"
#include "zipfolder.h"
#include "siftcmp.h"
#include "middlebox.h"

using std::string;

bool dedup_switch;

static const char* num2str(int num, int minSize = 0)
{
	static char buffer[32] = { 0 };
	buffer[0] = 0;
	if (minSize == 2)
	{
		if (num > -1 && num < 100)
		{
			if (num < 10)
			{
				snprintf(buffer, 32, "0%d", num);
			}
			else
			{
				snprintf(buffer, 32, "%d", num);
			}
		}
	}
	return buffer;
}

static void printTime(const hrtime& b, const hrtime& e, string& info)
{
	if (e.second - b.second < 30)
	{
		eprintf("%s use time %d us. \n", info.c_str(), time_elapsed_in_us(&b, &e));
	}
	else
	{
		eprintf("%s use time %d s. \n", info.c_str(), e.second - b.second);
	}

}

// test cases go here
void ecall_entrance(int id, const char *path, int count, int dedup)
{
	hrtime start_time, end_time;

	int netGetTime, netPutTime;

	string dedupStr;

	if (dedup != 0)
	{
		dedup_switch = true;
		dedupStr.append("With dedup, ");
	}
	else
	{
		dedup_switch = false;
		dedupStr.append("Without dedup, ");
	}


	switch (id)
	{
		//zlib
	case 1:
	{
		get_time(&start_time);
		zipfolder_run(path);
		get_time(&end_time);
		printTime(start_time, end_time, dedupStr);

#ifndef USE_LOCAL_CACHE
		ocall_get_network_get_time(&netGetTime);
		ocall_get_network_put_time(&netPutTime);
		eprintf("            network get use %d us, network put use %d us.\n\n", netGetTime, netPutTime);
#endif // !USE_LOCAL_CACHE


		break;
	}
	//pcre
	case 2:
	{
		clearCounter();
		get_time(&start_time);
		middlebox_run(path, count);
		get_time(&end_time);
		printTime(start_time, end_time, dedupStr);

#ifndef USE_LOCAL_CACHE
		ocall_get_network_get_time(&netGetTime);
		ocall_get_network_put_time(&netPutTime);
		eprintf("            network get use %d us, network put use %d us.\n\n", netGetTime, netPutTime);
#endif // !USE_LOCAL_CACHE


		break;
	}
	//mapreduce bow
	case 3:
	{
		char ** allFiles = new char*[count];
		for (int i = 0; i < count; i++)
		{
			allFiles[i] = new char[64];
			snprintf(allFiles[i], 64, "%s%s", path, num2str(i + 1, 2));
		}


		get_time(&start_time);
		bow_run(count, allFiles);
		get_time(&end_time);
		printTime(start_time, end_time, dedupStr);

#ifndef USE_LOCAL_CACHE
		ocall_get_network_get_time(&netGetTime);
		ocall_get_network_put_time(&netPutTime);
		eprintf("            network get use %d us, network put use %d us.\n\n", netGetTime, netPutTime);
#endif // !USE_LOCAL_CACHE

		break;
	}
	//sift
	case 4:
	{
		get_time(&start_time);
		siftcmp_run(path);
		get_time(&end_time);
		printTime(start_time, end_time, dedupStr);

#ifndef USE_LOCAL_CACHE
		ocall_get_network_get_time(&netGetTime);
		ocall_get_network_put_time(&netPutTime);
		eprintf("            network get use %d us, network put use %d us.\n\n", netGetTime, netPutTime);
#endif // !USE_LOCAL_CACHE

		break;
	}
	// put and get 
	case 5:
	{
		ocall_put_get_time();
		break;
	}// end case 5
	}

}
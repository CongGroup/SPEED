#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctime>
#include <random>
#include <sys/types.h>
#include <fcntl.h>
#include <cstdint>
#include <fstream>
#include <sstream>


//
//
//int ocall_open(const char* filename, int mode) {
//
//	return open(filename, (mode_t)mode);
//}
//
//int ocall_read(int file, void *buf, unsigned int size)
//{
//	return read(file, buf, size);
//}
//
//int ocall_write(int file, void *buf, unsigned int size)
//{
//	return write(file, buf, size);
//}
//
//void ocall_close(int file)
//{
//	close(file);
//}

/* OCall functions */
//void ocall_print_string(const char *str)
//{
//	/* Proxy/Bridge will check the length and null-terminate
//	* the input string to prevent buffer overflow.
//	*/
//	printf("%s", str);
//}
//
//void ocall_get_time(int *second, int *nanosecond)
//{
//	timespec wall_clock;
//	clock_gettime(CLOCK_REALTIME, &wall_clock);
//	*second = wall_clock.tv_sec;
//	*nanosecond = wall_clock.tv_nsec;
//}

void ocall_rand(int *rand_num, int mod)
{
	*rand_num = rand() % mod;
}

void ocall_get_data(int data_id, char *val, size_t* len)
{
	switch (data_id)
	{
	default:
	{
		val = 0;
		*len = 0;
		break;
	}
	}
}

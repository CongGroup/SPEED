/*
 * Copyright (C) 2011-2017 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */

#include <string>

#include "Enclave.h"
#include "Enclave_t.h"  /* print_string */

#include <zlib.h>
#include "siftpp_sgx/sift-driver.h"
#include "sysutils.h"
#include "Deduplicable.h"


using namespace std;


static void sift_printBlock(byte* p, size_t size)
{
	for (int i = 0; i < size; i++)
	{
		eprintf("%.2X ", p[i]);
	}
	eprintf("\n");
}



void testSift(const char* fileName)
{
	static hrtime start_time, end_time;

	char *textfile;
	int filesize;
	load_text_file(fileName, &textfile, &filesize);


	int width;
	int height;

	byte* pic_Data = (byte *)textfile;

	memcpy(&width, pic_Data, sizeof(int));
	pic_Data += sizeof(int);
	memcpy(&height, pic_Data, sizeof(int));
	pic_Data += sizeof(int);

	const int   O = -1;
	const int   S = 3;
	const int   omin = -1;
	const float sigman = 0.5f;
	const float sigma0 = 2.0f;
	const int	smin = -1;
	const int	smax = 4;

	sift_printBlock(pic_Data, 128);



	eprintf("######Begin test siftpp with pic:%s ######\n", fileName);



	get_time(&start_time);

	string output1(height*width * sizeof(float) * 4, 0);
	output1 = ::computeSift((const float*)pic_Data, width, height,
		sigman, sigma0, O, S, omin, smin, smax);

	get_time(&end_time);

	eprintf("##Without dedup total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));



	get_time(&start_time);

	Deduplicable<string, const  float*, int, int, float, float,
		int, int, int, int, int> dedup_sift("libsiftpp",
			"0.8.1", "computeSift");
	string output2 = dedup_sift((float*)pic_Data, width, height,
		sigman, sigma0, O, S, omin, smin, smax);

	get_time(&end_time);

	eprintf("##With dedup first time total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));



	get_time(&start_time);

	Deduplicable<string, const  float*, int, int, float, float,
		int, int, int, int, int> dedup_sift2("libsiftpp",
			"0.8.1", "computeSift");
	string output3 = dedup_sift2((float*)pic_Data, width, height,
		sigman, sigma0, O, S, omin, smin, smax);

	get_time(&end_time);

	eprintf("##With dedup second time total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));



	if (output1.size() != output2.size() ||
		output1.size() != output3.size())
	{
		eprintf("error: %d, %d, %d \n", output1.size(), output2.size(), output3.size());
	}
}

void testlibZ(string input, string output)
{
#define CHUNK 16384
#define CP_LEVEL 9 // highest compression level (hence slowest for our sake)

	static hrtime start_time, end_time;

	get_time(&start_time);
	{
		int processed = 0, compressed = 0;
		int ret, flush;
		unsigned have;
		z_stream strm;
		int in_size = input.size();
		byte *in = (byte *)input.data();
		output.reserve(in_size * 2);
		byte *out = (byte *)output.data();
		/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;

		ret = deflateInit_(&strm, CP_LEVEL, ZLIB_VERSION, (int)sizeof(z_stream));
		if (ret != Z_OK) {
			eprintf("[*] Compresison initializaion fails with error code %d!", ret);
			abort();
		}
		/* compress until end of file */
		do {
			if ((processed + CHUNK) < in_size) {
				strm.avail_in = CHUNK;
				flush = Z_NO_FLUSH;
			}
			else {
				strm.avail_in = in_size - processed;
				flush = Z_FINISH;
			}
			strm.next_in = &in[processed];

			/* run deflate() on input until output buffer not full, finish
			compression if all of source has been read in */
			do {
				strm.avail_out = CHUNK;
				strm.next_out = &out[compressed];

				ret = deflate(&strm, flush);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

				have = CHUNK - strm.avail_out;
				//if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				//    (void)deflateEnd(&strm);
				//    //return Z_ERRNO;
				//}
				compressed += have;
			} while (strm.avail_out == 0);
			assert(strm.avail_in == 0);     /* all input will be used */
			processed += CHUNK;
		}
		/* done when last data in file processed */
		while (flush != Z_FINISH);
		assert(ret == Z_STREAM_END);        /* stream will be complete */

											/* clean up and return */
		(void)deflateEnd(&strm);
		//return Z_OK;
		eprintf(" [%d] \n", compressed);
	}
	get_time(&end_time);
	eprintf("##Without dedup total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));

	get_time(&start_time);
	{
		int processed = 0, compressed = 0;
		int ret, flush;
		unsigned have;
		z_stream strm;
		int in_size = input.size();
		byte *in = (byte *)input.data();
		output.reserve(in_size * 2);
		byte *out = (byte *)output.data();
		/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;

		Deduplicable<int, z_stream_s*, int, const char *, int> dedupinit("libz","1.2.11","deflateInit_");
		ret = dedupinit(&strm, CP_LEVEL, ZLIB_VERSION, (int)sizeof(z_stream));
		if (ret != Z_OK) {
			eprintf("[*] Compresison initializaion fails with error code %d!", ret);
			abort();
		}
		/* compress until end of file */
		do {
			if ((processed + CHUNK) < in_size) {
				strm.avail_in = CHUNK;
				flush = Z_NO_FLUSH;
			}
			else {
				strm.avail_in = in_size - processed;
				flush = Z_FINISH;
			}
			strm.next_in = &in[processed];

			/* run deflate() on input until output buffer not full, finish
			compression if all of source has been read in */
			do {
				strm.avail_out = CHUNK;
				strm.next_out = &out[compressed];

				Deduplicable<int, z_stream_s*, int> dedupdo("libz", "1.2.11", "deflate");
				ret = dedupdo(&strm, flush);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

				have = CHUNK - strm.avail_out;
				//if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				//    (void)deflateEnd(&strm);
				//    //return Z_ERRNO;
				//}
				compressed += have;
			} while (strm.avail_out == 0);
			assert(strm.avail_in == 0);     /* all input will be used */
			processed += CHUNK;
		}
		/* done when last data in file processed */
		while (flush != Z_FINISH);
		assert(ret == Z_STREAM_END);        /* stream will be complete */

		Deduplicable<int, z_stream_s*> dedupend("libz", "1.2.11", "deflateEnd");
		/* clean up and return */
		(void)dedupend(&strm);
		//return Z_OK;
		eprintf(" [%d] \n", compressed);
	}
	get_time(&end_time);
	eprintf("##first dedup total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));


	get_time(&start_time);

	get_time(&end_time);
	eprintf("##second dedup total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));
}


void ecall_test()
{
	eprintf("Begin test.\n");

	//testlibZ(srcStr, dstStr);
	//
	//testSift("./data/sift/500K.pgm");
	//Deduplicable<string, string, int, double> dedup_sift2("libsiftpp","0.8.1", "addNumtoStr");
	//
	//printf("use 'cd ../speed_core/; ./comp_instance;' instead.");

	eprintf("End test.\n");

	return;
}

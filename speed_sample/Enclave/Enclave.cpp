
#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */

#include <string>

#include "Enclave.h"
#include "Enclave_t.h"  /* print_string */

#include <zlib.h>
#include <pcre.h>
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

	//sift_printBlock(pic_Data, 128);
	//eprintf("######Begin test siftpp with pic:%s ######\n", fileName);

	string output1;
	output1.assign(height*width * sizeof(float) * 4, '0');
	get_time(&start_time);
	{
		output1 = ::__inner_sift_test(2, 5, 7);
		//output1 = ::computeSift((const float*)pic_Data, width, height, sigman, sigma0, O, S, omin, smin, smax);
		eprintf("output size [%d]:%s \n", output1.size(), output1.c_str());
	}
	get_time(&end_time);

	eprintf("##Without dedup total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));


	string output2;
	output2.assign(height*width * sizeof(float) * 4, '0');
	get_time(&start_time);
	{
		Deduplicable<string, const  float*, int, int, float, float,
			int, int, int, int, int> dedup_sift("libsiftpp",
				"0.8.1", "computeSift");
		output2 = dedup_sift((float*)pic_Data, width, height,
			sigman, sigma0, O, S, omin, smin, smax);
		eprintf("output size [%d] \n", output2.size());
	}
	get_time(&end_time);

	eprintf("##With dedup first time total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));



	string output3;
	output3.assign(height*width * sizeof(float) * 4, '0');
	get_time(&start_time);
	{
	Deduplicable<string, const  float*, int, int, float, float,
		int, int, int, int, int> dedup_sift2("libsiftpp",
			"0.8.1", "computeSift");
	output3 = dedup_sift2((float*)pic_Data, width, height,
		sigman, sigma0, O, S, omin, smin, smax);
	eprintf("output size [%d] \n", output3.size());
	}
	get_time(&end_time);

	eprintf("##With dedup second time total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));

}

void testlibZ(string input)
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
		string output;
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
				//eprintf("[*] Compresison add %d!\n", have);
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
		//eprintf("compressed total [%d] \n", compressed);
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
		string output;
		output.reserve(in_size * 2);
		byte *out = (byte *)output.data();
		/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;

		Deduplicable<int, z_stream_s*, int, const char *, int> deflateInit_("libz","1.2.11","deflateInit_");
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

				Deduplicable<int, z_stream_s*, int> dedupdo("libz", "1.2.11", "deflate");
				ret = dedupdo(&strm, flush);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

				have = CHUNK - strm.avail_out;
				//if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				//    (void)deflateEnd(&strm);
				//    //return Z_ERRNO;
				//}
				compressed += have;
				//eprintf("[*] Compresison add %d!\n", have);
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
		//eprintf("compressed total [%d] \n", compressed);
	}
	get_time(&end_time);
	eprintf("##first dedup total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));


	get_time(&start_time);
	{
		int processed = 0, compressed = 0;
		int ret, flush;
		unsigned have;
		z_stream strm;
		int in_size = input.size();
		byte *in = (byte *)input.data();
		string output;
		output.reserve(in_size * 2);
		byte *out = (byte *)output.data();
		/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;

		Deduplicable<int, z_stream_s*, int, const char *, int> deflateInit_("libz", "1.2.11", "deflateInit_");
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

				Deduplicable<int, z_stream_s*, int> dedupdo("libz", "1.2.11", "deflate");
				ret = dedupdo(&strm, flush);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

				have = CHUNK - strm.avail_out;
				//if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				//    (void)deflateEnd(&strm);
				//    //return Z_ERRNO;
				//}
				compressed += have;
				//eprintf("[*] Compresison add %d!\n", have);
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
		//eprintf("compressed total [%d] \n", compressed);
	}
	get_time(&end_time);
	eprintf("##second dedup total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));
}

void testPcre(string str, string pattern)
{
#define PCRE_OVECTOR_SIZE 1000*3 // allow at most 1000 matches
	// output matching positions in the form {s_1,e_1,s_2,e_2,...,}
	// the last n/3 of the array are reserved by libpcre and not used
	int m_ovector[PCRE_OVECTOR_SIZE];
	const char     *m_error;
	int             m_erroffset;

	static hrtime start_time, end_time;

	get_time(&start_time);
	{
		pcre  *m_pcre;
		m_pcre = pcre_compile(pattern.c_str(), // pattern
			0,     // default options
			&m_error, &m_erroffset,  // errors
			NULL); // default character table
		int rc = pcre_exec(m_pcre, 0, // pcre engine
			str.c_str(), str.length(), // query string
			0, // starting offset
			0, // options
			m_ovector, PCRE_OVECTOR_SIZE); // output vector of matching positions

		//eprintf("pattern matching res [%d] \n", rc);
	}
	get_time(&end_time);
	eprintf("##Without dedup total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));

	get_time(&start_time);
	{
		pcre  *m_pcre;
		Deduplicable<pcre*, const char *, int, const char **, int *, const unsigned char *>
			dedup_compile("libpcre", "10.23", "pcre_compile");
		m_pcre = dedup_compile(pattern.c_str(), // pattern
			0,     // default options
			&m_error, &m_erroffset,  // errors
			NULL); // default character table

		Deduplicable<int, const pcre *, const pcre_extra *, const char*, int, int, int, int *, int>
			dedup_pcre("libpcre", "10.23", "pcre_exec");
		int rc = dedup_pcre(m_pcre, 0, // pcre engine
			str.c_str(), str.length(), // query string
			0, // starting offset
			0, // options
			m_ovector, PCRE_OVECTOR_SIZE); // output vector of matching positions

		//eprintf("pattern matching res [%d] \n", rc);
	}
	get_time(&end_time);
	eprintf("##first dedup total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));

	get_time(&start_time);
	{
		pcre  *m_pcre;
		Deduplicable<pcre*, const char *, int, const char **, int *, const unsigned char *>
			dedup_compile("libpcre", "10.23", "pcre_compile");
			m_pcre = dedup_compile(pattern.c_str(), // pattern
				0,     // default options
				&m_error, &m_erroffset,  // errors
				NULL); // default character table

		Deduplicable<int, const pcre *, const pcre_extra *, const char*, int, int, int, int *, int>
			dedup_pcre("libpcre", "10.23", "pcre_exec");
		int rc = dedup_pcre(m_pcre, 0, // pcre engine
			str.c_str(), str.length(), // query string
			0, // starting offset
			0, // options
			m_ovector, PCRE_OVECTOR_SIZE); // output vector of matching positions

		//eprintf("pattern matching res [%d] \n", rc);
	}
	get_time(&end_time);
	eprintf("##second dedup total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));
}


void ecall_test()
{
	eprintf("Begin test.\n");

	eprintf("Test zlib function\n");
	string srcStr;
	srcStr.assign("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()_+1234567890-=");
	testlibZ(srcStr);


	eprintf("\n\n\n");
	eprintf("Test pcre function\n");
	testPcre("hello,abcdddddddabceeeeeeeeabcde,hhhhhhh", "a(bc)");

	eprintf("End test.\n");
	return;
}

 
#include "zipfolder.h"
#include "stdlib.h"
#include "Enclave_t.h"
#include "sysutils.h"
#include "dedupTool.h"

#include <zlib.h>

static int zlibfile_without_dedup(char* input, char* output, int size)
{
#define CHUNK 16384
#define CP_LEVEL 9 

	int processed = 0, compressed = 0;
	int ret, flush;
	unsigned have;
	z_stream strm;
	int in_size = size;
	byte *in = (byte *)input;
	byte *out = (byte *)output;

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, CP_LEVEL);
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
		strm.next_in = (Bytef*)&in[processed];

		/* run deflate() on input until output buffer not full, finish
		compression if all of source has been read in */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = (Bytef*)&out[compressed];

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
	//assert(ret == Z_STREAM_END);        /* stream will be complete */

										/* clean up and return */
	(void)deflateEnd(&strm);

	return compressed;
}

#include "FunctionDB.h"

static int zlibfile_with_dedup(char* in, char* out, int size)
{
	DEDUP_FUNCTION_INIT;
	byte* pb;

	input_with_r_buffer_size = size + HASH_SIZE;
	input_with_r_buffer = new byte[input_with_r_buffer_size];
	memset(input_with_r_buffer, 0, input_with_r_buffer_size);

	output_buffer_size = size*1.2;
	output_buffer = new byte[output_buffer_size];
	memset(output_buffer, 0, output_buffer_size);
	pb = input_with_r_buffer;

	memcpy(pb, in, size);

	DEDUP_FUNCTION_QUERY;
	output_true_size = zlibfile_without_dedup(in,out,size);
	pb = output_buffer;
	memcpy(pb, out, output_true_size);

	DEDUP_FUNCTION_UPDATE;
	if (doDedup) {
		pb = output_buffer;
		memcpy(out, pb, output_true_size);
	}
	delete[] input_with_r_buffer;
	delete[] output_buffer;
	return output_true_size;
}

static string cmpressOneFile(const char* path)
{
	string fileContext = loadFiletoString(path);
	int fileLength = fileContext.size();
	char* outputBuffer = new char[(int)(fileLength*1.2)];
	int resLength = zlibfile_with_dedup((char*)fileContext.c_str(), outputBuffer, fileLength);
	return string(outputBuffer, resLength);
}

static string cmpressOneFile_dedup(const char* path)
{
	std::string hash = computeFileHash(path);
	std::string output;
	bool exist = queryByHash(hash);

	if (exist)
	{
		output = getResultByHash(hash);
	}
	else
	{
		cmpressOneFile(path);
		putResultByHash(hash, output);
	}

	return output;
}

extern bool dedup_switch;

static void compress_file(const char* path)
{
	const int compress_block_size = 2 * 1024 * 1024;

	int file_size = 0;
	ocall_file_size(&file_size, path);
	if (file_size > 0)
	{
		 // read file
		 char* file_in = (char*)malloc(file_size);
		 char* file_out = (char*)malloc(compress_block_size*1.2);

		 int fileid = 0;
		 ocall_open(&fileid, path, 0);
		 if (fileid < 0)
		 {
			 eprintf("%s error in open.\n", path);
			 return;
		 }

		 int size_to_read = file_size;
		 int cmp_size = 0;
		 while (size_to_read > 0)
		 {
			 int readRet = 0;
			 ocall_read(&readRet, fileid, file_in, size_to_read>compress_block_size ? compress_block_size : size_to_read);
			 if (readRet <0)
			 {
				 eprintf("%s error in read %d/%d\n", path, readRet, size_to_read);
			 }
			 size_to_read -= readRet;

			 if (dedup_switch)
			 {
				 cmp_size += zlibfile_with_dedup(file_in, file_out, readRet);
			 }
			 else
			 {
				 cmp_size += zlibfile_without_dedup(file_in, file_out, readRet);
			 }

		 }

		 ocall_close(fileid);


		 eprintf("%s after compress %d/%d\n", path, cmp_size, file_size);

		 free(file_in);
		 free(file_out);
	}
}




int zipfolder_run(const char * path)
{
	const int each_file_path_buffer_len = 200;
	const int max_file_count = 20;
	const int buffer_size = each_file_path_buffer_len * max_file_count;
	char(*buffer)[each_file_path_buffer_len] = (char(*)[each_file_path_buffer_len])malloc(buffer_size);
	int file_count= 0;
	ocall_read_dir(&file_count, path, (char*)buffer, max_file_count, buffer_size);
	if (file_count > 0)
	{
		for (int i = 0; i < file_count; i++)
		{
			compress_file((const char*)buffer);
			buffer++;
		}
	}
	return file_count;
}

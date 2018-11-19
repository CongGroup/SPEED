#include "zipfolder.h"
#include "stdlib.h"
#include "Enclave_t.h"
#include "sysutils.h"
#include "dedupTool.h"
#include "FunctionDB.h"
#include "zlib.h"
#include "Deduplicable.h"


// using zlib to compress file and return the result
static string zlib_compress_wrapper(const string& fileContext)
{
#define CHUNK 16384
#define CP_LEVEL 9 

	int fileLength = fileContext.size();
	const byte *in = (byte *)fileContext.c_str();
	byte *out = new byte[(int)(fileLength*1.2)];
	int processed = 0, compressed = 0;
	int ret, flush;
	unsigned have;
	z_stream strm;

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
		if ((processed + CHUNK) < fileLength) {
			strm.avail_in = CHUNK;
			flush = Z_NO_FLUSH;
		}
		else {
			strm.avail_in = fileLength - processed;
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
	while (flush != Z_FINISH);
	(void)deflateEnd(&strm);

	return string((char*)out,compressed);
}

extern bool dedup_switch;

static void compress_file(const char* path)
{
	const int compress_block_size = 4 * 1024 * 1024;

	int file_size = 0;
	ocall_file_size(&file_size, path);

	// read file
	char* file_in = (char*)malloc(compress_block_size + 1);
	char* file_out = (char*)malloc(compress_block_size*1.2);
	string str_in, str_out;

	if (file_size > 0)
	{
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
			 str_in.assign(file_in, readRet);
			 if (readRet <0)
			 {
				 eprintf("%s error in read %d/%d\n", path, readRet, size_to_read);
			 }
			 size_to_read -= readRet;

			 if (dedup_switch)
			 {
				 Deduplicable<string, const string&> dedup_func("libz", "1.2.11", "zlib_compress_wrapper", zlib_compress_wrapper);
				 str_out = dedup_func(str_in);
				 cmp_size += str_out.size();
			 }
			 else
			 {
				 str_out = zlib_compress_wrapper(str_in);
				 cmp_size += str_out.size();
			 }
		 }
		 ocall_close(fileid);
		 eprintf("%s after compress %d/%d\n", path, cmp_size, file_size);
	}
	else
	{
		eprintf("file %s is empty or not exist.\n", path);
	}

	free(file_in);
	free(file_out);
}

int zipfolder_run(const char * path)
{
	const int each_file_path_buffer_len = 128;
	char(*buffer_out)[each_file_path_buffer_len];
	
	int file_count= 0;
	ocall_read_dir(&file_count, path, (char**)&buffer_out, each_file_path_buffer_len);

	char(*buffer)[each_file_path_buffer_len] = (char(*)[each_file_path_buffer_len])new char[file_count*each_file_path_buffer_len+1];
	memcpy(buffer, buffer_out, file_count*each_file_path_buffer_len);
	
	eprintf("Find %d files in path %s\n", file_count, path);
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

#include "dedupTool.h"

#include "sysutils.h"
#include "Enclave_t.h"
#include "cstring"
#include "crypto.h"

string computeFileHash(const char * path)
{
	string fileContext = loadFiletoString(path);
	return hashString(fileContext);
}

string computeStringHash(const string& str)
{
	return hashString(str);
}

bool queryByHash(const string & hash)
{
	const int output_buffer_size = 10*1024*1024;
	metadata meta; 
	byte func_tag[HASH_SIZE] = { 0 };
	byte* outputBuffer = new byte[output_buffer_size];

	memcpy(func_tag, hash.c_str(), HASH_SIZE);

	int outputSize = 0;
	ocall_request_find(func_tag, (byte*)(&meta), outputBuffer, output_buffer_size, &outputSize);

	return outputSize>0;
}

string getResultByHash(const string & hash)
{
	const int output_buffer_size = 10 * 1024 * 1024;
	metadata meta;
	memset(&meta, 0, sizeof(meta));
	byte func_tag[HASH_SIZE] = { 0 };
	byte enc_key[ENC_KEY_SIZE] = { 0 }; 
	byte hashr[HASH_SIZE] = { 0 };
	byte* outputBuffer = new byte[output_buffer_size];

	memcpy(func_tag, hash.c_str(), HASH_SIZE);

	int outputSize = 0;
	ocall_request_find(func_tag, (byte*)(&meta), outputBuffer, output_buffer_size, &outputSize);

	for (int i = 0; i < ENC_KEY_SIZE; ++i)
		enc_key[i] = hashr[i] ^ meta.enc_key[i];

	veri_dec(enc_key, ENC_KEY_SIZE, outputBuffer, outputSize, outputBuffer, meta.mac);

	return string((char*)outputBuffer, outputSize);
}

void putResultByHash(const string & hash, const string & result)
{
	const int output_buffer_size = 10 * 1024 * 1024;
	metadata meta;
	memset(&meta, 0, sizeof(meta));
	byte func_tag[HASH_SIZE] = { 0 };
	byte* outputBuffer = new byte[output_buffer_size];

	memcpy(func_tag, hash.c_str(), HASH_SIZE);
	memcpy(outputBuffer, result.c_str(), result.size());

	int outputSize = result.size();
	
	byte enc_key[ENC_KEY_SIZE] = { 0 }; 
	byte hashr[HASH_SIZE] = { 0 };

	key_gen(enc_key, ENC_KEY_SIZE);
	auth_enc(enc_key, ENC_KEY_SIZE, outputBuffer, outputSize, outputBuffer, meta.mac);

	for (int i = 0; i < ENC_KEY_SIZE; ++i)
		meta.enc_key[i] = enc_key[i] ^ hashr[i];

	ocall_request_put(func_tag, (byte*)(&meta), outputBuffer, outputSize);


}

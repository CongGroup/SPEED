#pragma once
#include <string>
#include <cstring>
#include <functional>

#include "Enclave_t.h"

#include "../common/config.h"
#include "../common/data_type.h"
#include "crypto.h"


template<typename _Res, typename... _ArgTypes>
class FunctionDB
{
public:
	virtual std::function<_Res(_ArgTypes...)>* getFunction(std::string name) = 0;
};


#define DEDUP_FUNCTION_INIT 	byte func_tag[HASH_SIZE] = {0};\
metadata meta;\
bool doDedup = false;\
byte* input_with_r_buffer = 0;\
int input_with_r_buffer_size = 0;\
byte* output_buffer = 0;\
int output_buffer_size = 0;\
int output_true_size = 0;\
byte enc_key[ENC_KEY_SIZE] = {0};\
byte hashr[HASH_SIZE] = {0};

#define DEDUP_FUNCTION_QUERY 	::hash(input_with_r_buffer, input_with_r_buffer_size - RAND_SIZE, func_tag);\
::ocall_request_find(func_tag, (byte*)(&meta), output_buffer, output_buffer_size, &output_true_size);\
if (output_true_size > 0){\
doDedup = true;\
memcpy(input_with_r_buffer + input_with_r_buffer_size - RAND_SIZE, meta.r, RAND_SIZE);\
::hash(input_with_r_buffer, input_with_r_buffer_size, hashr);\
for (int i = 0; i < ENC_KEY_SIZE; ++i)\
enc_key[i] = hashr[i] ^ meta.enc_key[i];\
veri_dec(enc_key, ENC_KEY_SIZE, output_buffer, output_true_size, output_buffer, meta.mac);}\
else{\

#define DEDUP_FUNCTION_UPDATE 		draw_rand(meta.r, RAND_SIZE);\
memcpy(input_with_r_buffer + input_with_r_buffer_size - RAND_SIZE, meta.r, RAND_SIZE);\
key_gen(enc_key, ENC_KEY_SIZE);\
auth_enc(enc_key, ENC_KEY_SIZE, output_buffer, output_true_size, output_buffer, meta.mac);\
::hash(input_with_r_buffer, input_with_r_buffer_size, hashr);\
for (int i = 0; i < ENC_KEY_SIZE; ++i)\
meta.enc_key[i] = enc_key[i] ^ hashr[i];\
::ocall_request_put(func_tag, (byte*)(&meta), output_buffer, output_true_size);}

#define COPY_OBJECT(pointer, obj) ::memcpy((pointer), &(obj), sizeof(obj));\
(pointer) += sizeof(obj);

#define READ_OBJECT(pointer, obj) ::memcpy(&(obj), (pointer), sizeof(obj));\
(pointer) += sizeof(obj);




/*dedup function template with Macros
ReturnType FunctionName(Args...)
{
DEDUP_FUNCTION_INIT;
// TODO: init input buffer and size
//		 init output buffer and size
//		 set intput value
ReturnType returnValue;
byte* pb;
input_with_r_buffer_size = inputSize + HASH_SIZE;
input_with_r_buffer = new byte[input_with_r_buffer_size];
memset(input_with_r_buffer, 0, input_with_r_buffer_size);
output_buffer_size = outputSize;
output_buffer = new byte[output_buffer_size];
memset(output_buffer, 0, output_buffer_size);
pb = input_with_r_buffer;
COPY_OBJECT(pb, object);
memcpy(pb, Args..., sizeof...(Args));

DEDUP_FUNCTION_QUERY;
// TODO: compute output
//		 set output value and size
returnValue = doFunction();
output_true_size = Size;
pb = output_buffer;
COPY_OBJECT(pb, object);
memcpy(pb, Output, OutputSize);

DEDUP_FUNCTION_UPDATE;
//TODO build return value
if(doDedup){
	pb = output_buffer;
	READ_OBJECT(pb, returnValue);
	memcpy(returnValue, pb, object);}
delete[] input_with_r_buffer;
delete[] output_buffer;
return returnValue;
}
*/


/*dedup function template
ReturnType FunctionName(Args...)
{
	byte func_tag[HASH_SIZE] = 0;
	metadata meta;
	ReturnType res;

	byte* input_with_r_buffer = 0;
	int input_with_r_buffer_size = 0;

	byte* output_buffer = 0;
	int output_buffer_size = 0;
	int output_true_size = 0;

	byte enc_key[ENC_KEY_SIZE] = 0;
	byte hashr[HASH_SIZE] = 0;

	// 1. gen input tag for this function
	// TODO: compute input tag
	memcpy(input_with_r_buffer, Args..., sizeof...(Args))
	::hash(input_with_r_buffer, input_with_r_buffer_size - RAND_SIZE, hashr);

	// 2. query cache
	ocall_request_find(func_tag, (byte*)(&meta), output_buffer, output_buffer_size, &output_true_size);

	if (output_true_size > 0)
	{
		// if hit cache.
		// 3. get R
		memcpy(input_with_r_buffer + input_with_r_buffer_size - RAND_SIZE, meta.r, RAND_SIZE);
		// 4. get dec key
		::hash(input_with_r_buffer, input_with_r_buffer_size, hashr);
		for (int i = 0; i < ENC_KEY_SIZE; ++i)
			enc_key[i] = hashr[i] ^ meta.enc_key[i];
		// 5. decrypt output
		veri_dec(enc_key, ENC_KEY_SIZE, output_buffer, output_true_size, output_buffer, meta.mac);
	}
	else
	{
		// if miss cache
		// 3. do function
		// TODO: use input compute the output
		output_true_size = outputSize;
		// 4. gen R
		draw_rand(meta.r, RAND_SIZE);
		memcpy(input_with_r_buffer + input_with_r_buffer_size - RAND_SIZE, meta.r, RAND_SIZE);
		// 5. gen enc key
		key_gen(enc_key, ENC_KEY_SIZE);
		// 6. encrypt output
		auth_enc(enc_key, ENC_KEY_SIZE, output_buffer, output_true_size, output_buffer, meta.mac);
		// 7. put cache
		::hash(input_with_r_buffer, input_with_r_buffer_size, hashr);
		for (int i = 0; i < ENC_KEY_SIZE; ++i)
			meta.enc_key[i] = enc_key[i] ^ hashr[i];
		ocall_request_put(func_tag, (byte*)(&meta), output_buffer, output_true_size);
	}
	// TODO: build return value with buffer
	return ReturnType(output_buffer, output_true_size);
}
*/
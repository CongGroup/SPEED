#pragma once
#include "FunctionDB.h"

#include "../trusted/siftpp_sgx/sift-driver.h"

#include <cstring>

template<typename _Res, typename... _ArgTypes>
class siftppDB_0_8_1 : public FunctionDB<_Res, _ArgTypes...>
{
public:
	siftppDB_0_8_1() {}
	~siftppDB_0_8_1() {}

	virtual std::function<_Res(_ArgTypes...)>* getFunction(std::string name)
	{
		if (name == std::string("computeSift"))
		{
			std::function<std::string(const float*, int, int, float, float, int, int, int, int, int)>* pf
				= new std::function<std::string(const float*, int, int, float, float, int, int, int, int, int)>();
			*pf = std::bind(&siftppDB_0_8_1::computeSift, this, std::placeholders::_1, std::placeholders::_2
				, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6
				, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10);
			return (std::function<_Res(_ArgTypes...)>*)pf;
		}
		else if (name == std::string("addNumtoStr") && sizeof...(_ArgTypes) == 3)
		{
			std::function<std::string(std::string, int, double)>* pf = new std::function<std::string(std::string, int, double)>;
			*pf = std::bind(&siftppDB_0_8_1::addNumtoStr, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			return (std::function<_Res(_ArgTypes...)>*)pf;
		}
		return nullptr;
	}

	std::string addNumtoStr(std::string basestr, int i, double d)
	{
		return basestr.append(std::to_string(i)).append(std::to_string(d));
	}
	
	std::string computeSift(const float* _im_pt, int _width, int _height,
		float _sigman, float _sigma0, int _O, int _S, int _omin, int _smin, int _smax)
	{
		byte func_tag[HASH_SIZE]= { 0 };
		metadata meta;

		byte* input_with_r_buffer = 0;
		int input_with_r_buffer_size = 0;

		byte* output_buffer = 0;
		int output_buffer_size = 0;
		int output_true_size = 0;

		byte enc_key[ENC_KEY_SIZE] = { 0 };
		byte hashr[HASH_SIZE] = { 0 };

		// 1. gen input tag for this function
		input_with_r_buffer_size = _width * _height * sizeof(float) + sizeof(float) * 2 + sizeof(int) * 7 + HASH_SIZE;
		input_with_r_buffer = new byte[input_with_r_buffer_size];
		output_buffer_size = _width * _height * sizeof(float) * 4;
		output_buffer = new byte[output_buffer_size];


		byte* pb = input_with_r_buffer;
		::memcpy(pb, _im_pt, _width*_height * sizeof(float));
		pb += _width * _height * sizeof(float);
		COPY_OBJECT(pb, _width);
		COPY_OBJECT(pb, _height);
		COPY_OBJECT(pb, _sigman);
		COPY_OBJECT(pb, _sigma0);
		COPY_OBJECT(pb, _O);
		COPY_OBJECT(pb, _S);
		COPY_OBJECT(pb, _omin);
		COPY_OBJECT(pb, _smin);
		COPY_OBJECT(pb, _smax);

		::hash(input_with_r_buffer, input_with_r_buffer_size - RAND_SIZE, func_tag);

		// 2. query cache
		::ocall_request_find(func_tag, (byte*)(&meta), output_buffer, output_buffer_size, &output_true_size);

		if (output_true_size > 0)
		{
			// if hit cache.
			// 3. get R
			memcpy(pb, meta.r, RAND_SIZE);
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
			std::string res = ::__inner_sift_test(2, 5, 7);
			//std::string res = ::computeSift(_im_pt, _width, _height, _sigman, _sigma0, _O, _S, _omin, _smin, _smax);
			//if (res.size() < 10 || res.size()>100000)
			//{
			//	res = "hello world";
			//}
			output_true_size = res.size();
			memcpy(output_buffer, res.data(), output_true_size);
			//memcpy(output_buffer, output_buffer, 100);
			// 4. gen R
			draw_rand(meta.r, RAND_SIZE);
			memcpy(pb, meta.r, RAND_SIZE);
			// 5. gen enc key
			key_gen(enc_key, ENC_KEY_SIZE);
			// 6. encrypt output
			auth_enc(enc_key, ENC_KEY_SIZE, output_buffer, output_true_size, output_buffer, meta.mac);
			// 7. put cache
			::hash(input_with_r_buffer, input_with_r_buffer_size, hashr);
			for (int i = 0; i < ENC_KEY_SIZE; ++i)
				meta.enc_key[i] = enc_key[i] ^ hashr[i];
			::ocall_request_put(func_tag, (byte*)(&meta), output_buffer, output_true_size);
		}
		std::string res;
		if (output_true_size > 0)
		{
			res.assign((char*)output_buffer, output_true_size);
		}
		else
		{
			res = "";
		}
		
		delete[] input_with_r_buffer;
		delete[] output_buffer;
		return res;
	}

};


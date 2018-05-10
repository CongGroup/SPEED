#pragma once
#include "FunctionDB.h"

#include "../trusted/pcre_sgx/pcre.h"

#include <cstring>

template<typename _Res, typename... _ArgTypes>
class libpcreDB_10_23 : public FunctionDB<_Res, _ArgTypes...>
{
public:
	libpcreDB_10_23() {}
	~libpcreDB_10_23() {}

	virtual std::function<_Res(_ArgTypes...)>* getFunction(std::string name)
	{
		if (name == std::string("pcre_compile"))
		{
			std::function<pcre *(const char * , int , const char ** , int * , const unsigned char * )>* pf
				= new std::function<pcre *(const char *, int, const char **, int *, const unsigned char *)>();
			*pf = std::bind(&libpcreDB_10_23<_Res, _ArgTypes...>::pcre_compile, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
			return (std::function<_Res(_ArgTypes...)>*)pf;
		}
		else if (name == std::string("pcre_exec"))
		{
			std::function<int(const pcre *, const pcre_extra *, const char*, int, int, int, int *, int)>* pf
				= new std::function<int(const pcre *, const pcre_extra *, const char*, int, int, int, int *, int)>();
			*pf = std::bind(&libpcreDB_10_23<_Res, _ArgTypes...>::pcre_exec, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8);
			return (std::function<_Res(_ArgTypes...)>*)pf;
		}
		return nullptr;
	}

	pcre *pcre_compile(const char * pch, int num, const char ** ppch, int * pi,const unsigned char * puc)
	{
		// this func can`t be dedup
		return ::pcre_compile(pch, num, ppch, pi, puc);
	}

	int  pcre_exec(const pcre * pengine, const pcre_extra * pextra, const char* pch, int len, int offset, int options, int * poutput, int out_size)
	{
		DEDUP_FUNCTION_INIT;
		int res;
		input_with_r_buffer_size = len + sizeof(int)*2 + HASH_SIZE;
		input_with_r_buffer = new byte[input_with_r_buffer_size];
		memset(input_with_r_buffer, 0, input_with_r_buffer_size);

		output_buffer_size = out_size + sizeof(int);
		output_buffer = new byte[output_buffer_size];
		memset(output_buffer, 0, output_buffer_size);

		byte* pb = input_with_r_buffer;
		COPY_OBJECT(pb, offset);
		COPY_OBJECT(pb, options);
		memcpy(pb, pch, len);

		DEDUP_FUNCTION_QUERY;
		memset(poutput, 0, out_size*sizeof(int));
		res = ::pcre_exec(pengine, pextra, pch, len, offset, options, poutput, out_size);
		if (res >=0)
		{
			output_true_size = (res + 1) * sizeof(int) * 2 + sizeof(res);
			pb = output_buffer;
			COPY_OBJECT(pb, res);
			memcpy(pb, poutput, (res + 1) * sizeof(int) * 2);
		}
		else
		{
			//pcre error
			output_true_size = sizeof(res);
			pb = output_buffer;
			COPY_OBJECT(pb, res);
		}

		DEDUP_FUNCTION_UPDATE;
		if (doDedup)
		{
			pb = output_buffer;
			READ_OBJECT(pb, res);
			if (res > 0)
			{
				memcpy(poutput, pb, (res + 1) * sizeof(int) * 2);
			}
		}
		delete[] input_with_r_buffer;
		delete[] output_buffer;
		return res;
	}
  
};


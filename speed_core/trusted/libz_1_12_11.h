#pragma once
#include "FunctionDB.h"

#include <zlib.h>
#include <cstring>

template<typename _Res, typename... _ArgTypes>
class libzDB_1_12_11 : public FunctionDB<_Res, _ArgTypes...>
{
public:
	libzDB_1_12_11() {}
	~libzDB_1_12_11() {}

	virtual std::function<_Res(_ArgTypes...)>* getFunction(std::string name)
	{
		if (name == std::string("deflateInit"))
		{
			std::function<int(z_stream_s*, int, const char *, int)>* pf
				= new std::function<int(z_stream_s*, int, const char *, int)>();
			*pf = std::bind(&libzDB_1_12_11<_Res, _ArgTypes...>::deflateInit_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			return (std::function<_Res(_ArgTypes...)>*)pf;
		}
		else if (name == std::string("deflate"))
		{
			std::function<int(z_stream_s*, int)>* pf
				= new std::function<int(z_stream_s*, int)>();
			*pf = std::bind(&libzDB_1_12_11<_Res, _ArgTypes...>::deflate, this, std::placeholders::_1, std::placeholders::_2);
			return (std::function<_Res(_ArgTypes...)>*)pf;
		}
		else if (name == std::string("deflateEnd"))
		{
			std::function<int(z_stream_s*)>* pf = new std::function<int(z_stream_s*)>;
			*pf = std::bind(&libzDB_1_12_11<_Res, _ArgTypes...>::deflateEnd, this, std::placeholders::_1);
			return (std::function<_Res(_ArgTypes...)>*)pf;
		}
		return nullptr;
	}


	int deflateInit_(z_stream_s* strm, int level, const char *version, int stream_size)
	{
		return ::deflateInit_(strm, level, version, stream_size);
	}

	int deflate(z_stream_s* stream, int num)
	{
		return ::deflate(stream, num);
	}

	int deflateEnd(z_stream_s* stream)
	{
		return ::deflateEnd(stream);
	}
};


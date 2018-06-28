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
		if (name == std::string("deflateInit_"))
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
		// this func can`t be dedup
		return ::deflateInit_(strm, level, version, stream_size);
	}

	int deflate(z_stream_s* stream, int num)
	{
		DEDUP_FUNCTION_INIT;
		const int defaultRet = 0xffffff;
		int ret = defaultRet;
		uint32_t din;
		uint32_t dout;

		input_with_r_buffer_size = stream->avail_in + sizeof(int)*3 + HASH_SIZE;
		input_with_r_buffer = new byte[input_with_r_buffer_size];
		memset(input_with_r_buffer, 0, input_with_r_buffer_size);

		output_buffer_size = stream->avail_out + sizeof(int) * 3;
		output_buffer = new byte[output_buffer_size];
		memset(output_buffer, 0, output_buffer_size);


		byte* pb = input_with_r_buffer;
		memcpy(pb, stream->next_in, stream->avail_in);
		pb += stream->avail_in;

		COPY_OBJECT(pb, num);
		COPY_OBJECT(pb, stream->avail_in);
		COPY_OBJECT(pb, stream->avail_out);

		DEDUP_FUNCTION_QUERY;

		uint32_t avail_in = stream->avail_in;
		uint32_t avail_out = stream->avail_out;
		ret = ::deflate(stream, num);
		din = avail_in - stream->avail_in;
		dout = avail_out - stream->avail_out;

		if (dout == 0)
		{
			return ret;
		}

		output_true_size = output_buffer_size;

		pb = output_buffer;

		COPY_OBJECT(pb, ret);
		COPY_OBJECT(pb, din);
		COPY_OBJECT(pb, dout);

		memcpy(pb, stream->next_out - dout, dout);
		pb += avail_out;

		DEDUP_FUNCTION_UPDATE;

		if (ret == defaultRet)
		{
			pb = output_buffer;

			READ_OBJECT(pb, ret);
			READ_OBJECT(pb, din);
			READ_OBJECT(pb, dout);

			memcpy(stream->next_out, pb, dout);

			stream->next_in += din;
			stream->avail_in -= din;
			stream->next_out += dout;
			stream->avail_out -= dout;
		}
		return ret;
	}

	int deflateEnd(z_stream_s* stream)
	{
		// this func can`t be dedup
		return ::deflateEnd(stream);
	}
};


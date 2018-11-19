#pragma once

#include "FunctionDB.h"
#include "tuple"

template<typename Object>
static int obj2buf(byte* buff, const Object& obj)
{
	int size = sizeof(obj);
	if (buff)
	{
		COPY_OBJECT(buff, obj);
	}
	return size;
}

template <>
int obj2buf(byte* buff, const std::string& obj)
{
	int size = obj.size();
	if (buff)
	{
		COPY_POINTER(buff, obj.data(), obj.size());
	}
	return size;
}

template<typename Object>
static Object buf2obj(const byte* buff, int size)
{
	Object obj;
	int n = size > sizeof(obj) ? sizeof(obj) : size;
	memcpy(&obj, buff, n);

	return obj;
}

template<>
std::string buf2obj(const byte* buff, int size)
{
	return std::string((const char*)buff, size);
}

template<class Last>
static int setInputBuffer(byte* buff, Last obj)
{
	return obj2buf(buff, obj);
}

template<class First, class... Args>
static int setInputBuffer(byte* buff, First obj, Args... params)
{
	int size = obj2buf(buff, obj);
	if (buff)
	{
		buff += size;
	}
	size += setInputBuffer(buff, params...);
	return size;
}

template< class ReturnType, class... Args>
class Deduplicable
{
public:
	Deduplicable(std::string _libName, std::string _libVersion, std::string _functionName, ReturnType(*_funcPointer)(Args...))
		: libName(_libName), libVersion(_libVersion), functionName(_functionName), funcPointer(_funcPointer)
	{
		output_buffer_scale_size = 1.0;
		output_minimum_size = 0;
	}

	double get_output_scale()
	{
		return output_buffer_scale_size;
	}

	int get_output_minimum_size()
	{
		return output_minimum_size;
	}

	void set_output_scale(double d)
	{
		if (d > 0)
		{
			output_buffer_scale_size = d;
		}
	}

	void get_output_minimum_size(int n)
	{
		output_minimum_size = n;
	}

	ReturnType operator()(Args... params)
	{
		DEDUP_FUNCTION_INIT;
		ReturnType res;
		input_with_r_buffer_size = setInputBuffer(0, params...) + HASH_SIZE;
		input_with_r_buffer = new byte[input_with_r_buffer_size];
		memset(input_with_r_buffer, 0, input_with_r_buffer_size);

		output_buffer_size = input_with_r_buffer_size* output_buffer_scale_size;// sizeof(ReturnType);
		if (output_buffer_size < output_minimum_size)
		{
			output_buffer_size = output_minimum_size;
		}
		output_buffer = new byte[output_buffer_size];
		//memset(output_buffer, 0, output_buffer_size);

		setInputBuffer(input_with_r_buffer, params...);

		DEDUP_FUNCTION_QUERY;
		res = (*funcPointer)(params...);
		output_true_size = obj2buf(output_buffer, res);

		if (output_true_size > output_buffer_size)
		{
			eprintf("Error: dedup output buffer too small, %d/%d*%lf\n.", output_true_size, input_with_r_buffer_size, output_buffer_scale_size);
		}

		DEDUP_FUNCTION_UPDATE;
		if (doDedup)
		{
			res = buf2obj<ReturnType>(output_buffer, output_true_size);
		}
		delete[] input_with_r_buffer;
		delete[] output_buffer;
		return res;
	}

	std::string getLibName() { return libName; }
	std::string getLibVersion() { return getLibVersion; }
	std::string getFunctionName() { return functionName; }

private:
	ReturnType(*funcPointer)(Args...);
	std::string libName;
	std::string libVersion;
	std::string functionName;

	double output_buffer_scale_size;
	int output_minimum_size;

};

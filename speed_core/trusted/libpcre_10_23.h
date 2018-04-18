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
		return ::pcre_compile(pch, num, ppch, pi, puc);
	}

	int  pcre_exec(const pcre * pengine, const pcre_extra * pextra, const char* pch, int num1, int num2, int num3, int * pnum, int num4)
	{
		return ::pcre_exec(pengine, pextra, pch, num1, num2, num3, pnum, num4);
	}
  
};


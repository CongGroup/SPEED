#pragma once

#include "FunctionDB.h"

#include "siftppDB_0_8_1.h"
#include "libz_1_12_11.h"
#include "libpcre_10_23.h"

template<typename _Res, typename... _ArgTypes>
class FunctionDBFactory
{
public:
	static FunctionDB<_Res, _ArgTypes...>* getDB(std::string libName, std::string version, int contextID)
	{
		if (libName == std::string("libpcre"))
		{
			if (version == std::string("10.23"))
			{
				return new libpcreDB_10_23<_Res, _ArgTypes...>();
			}
		}
		else if (libName == std::string("libz"))
		{
			if (version == std::string("1.2.11"))
			{
				return new libzDB_1_12_11<_Res, _ArgTypes...>();
			}
		}
		else if (libName == std::string("libsiftpp"))
		{
			if (version == std::string("0.8.1"))
			{
				// use siftppDB_0_8_1(contextID) if needed
				return new siftppDB_0_8_1<_Res, _ArgTypes...>();
			}
		}
		return nullptr;
	}

};


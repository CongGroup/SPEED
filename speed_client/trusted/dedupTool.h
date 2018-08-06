#pragma once

#include <string>

using std::string;

string computeFileHash(const char* path);

string computeStringHash(const string& str);

bool queryByHash(const string& hash);

string getResultByHash(const string& hash);

void putResultByHash(const string& hash, const string& result);


//dedup function template with Macros
//string FunctionName(string intput)
//{
//	string returnValue;
//
//	std::string hash = computeFileHash(intput);
//	std::string hash = computeStringHash(intput);
//
//	bool exist = queryByHash(hash);
//
//	if (exist)
//	{
//		returnValue = getResultByHash(hash);
//	}
//	else
//	{
//		returnValue = doFunction(Args...);
//		putResultByHash(hash, returnValue);
//	}
//
//	return returnValue;
//}







#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

class dataType
{
public:
	// 0 means ignore, 1 means in, 2 means out
	int type;
	// if the name is none, means return value
	string typeName;
	bool isString;
	bool isPointer;
	// if is a Pointer
	int dataSize;
};

class funcType
{
public:
	// the params size is count of funcParam + 1(return type)
	vector<dataType> params;
	
	string funcName;

	string funcDedupName;

	string funcLibraryName;

	string funcLibraryVersion;
};





class dedupConfigType
{
public:
	string configName;

	vector<funcType> funcs;
	string swtichName;
	int localOnly;
};
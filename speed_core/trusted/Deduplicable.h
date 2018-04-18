#pragma once

#include "FunctionDBFactory.h"

template< class ReturnType, class... Args>
class Deduplicable
{
public:
	Deduplicable(std::string _libName, std::string _libVersion, std::string _functionName, int _ctxID = 0)
		: libName(_libName), libVersion(_libVersion), functionName(_functionName), contextID(_ctxID), functionObject(nullptr)
	{
		FunctionDB<ReturnType, Args...>* lib = FunctionDBFactory<ReturnType, Args...>::getDB(libName, libVersion, contextID);

		functionObject = lib->getFunction(functionName);
	}


	ReturnType operator()(Args... params)
	{
		return (*functionObject)(params...);
	}

	std::string getLibName() { return libName; }
	std::string getLibVersion() { return getLibVersion; }
	std::string getFunctionName() { return functionName; }
	int getContextID() { return contextID; }


private:
	std::function<ReturnType(Args...)>* functionObject;
	std::string libName;
	std::string libVersion;
	std::string functionName;
	int contextID;
};


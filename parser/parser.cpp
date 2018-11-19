
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

#include "parser.h"

using namespace std;

/*--------------tool functions------------*/
// del all repetitive space and '\t'
string trim(const string& src);
// del all delim char in src
string clear(const string& src, string delim = string(" =:"));
// justify the src begin is str
bool beginWith(const string& src, const string& str);


/*--------------parse functions-----------*/
// parse config file
dedupConfigType parseConfigFile(string configFile);
// use config replace source file
void replaceSourceWithDedupConfig(const dedupConfigType config, string srcFileName, string dstFileName);

int main2(int argc, char** argv)
{
	// check param
	if (argc != 3)
	{
		printf("Usage: parser [tmpFile] [cppFile], exit.\n");
		return 0;
	}

	dedupConfigType dedupConfig;

	string templateName(argv[1]);
	string sourceName(argv[2]);

	// load config
	dedupConfig = parseConfigFile(templateName);

	printf("parser config file and load %d functions.\n", (int)dedupConfig.funcs.size());


	// handle source file
	string outputSourceName = string(sourceName).append(".tpp");
	replaceSourceWithDedupConfig(dedupConfig, sourceName, outputSourceName);

	printf("paraser output source file %s.\n", outputSourceName.c_str());

	// end
	getchar();
	return 0;

}

dedupConfigType parseConfigFile(string configFile)
{
	dedupConfigType dedupConfig;

	dedupConfig.configName = configFile;

	const char* (supportAttr)[] = {
		"libName",
		"version"
	};

	ifstream fin(configFile, ios::in);

	string oneLineStr, tempStr, attrStr;
	int leftPos, rightPos, blankPos;
	int lineNo = 0;

	string currentLibName;
	string currentVersion;

	while (fin.good())
	{
		lineNo++;

		getline(fin, oneLineStr);

		// empty line
		if (oneLineStr.find('[') == string::npos && oneLineStr.find('(') == string::npos)
		{
			continue;
		}

		// comment line
		tempStr = trim(oneLineStr);
		if (beginWith(tempStr, "//"))
		{
			continue;
		}

		leftPos = tempStr.find('[');
		rightPos = tempStr.find(']');
		if (leftPos != string::npos && rightPos != string::npos)
		{
			attrStr = tempStr.substr(leftPos + 1, rightPos - 1);
			//cout << "attrStr is " << attrStr << endl;

			if (attrStr.substr(0, strlen(supportAttr[0])) == supportAttr[0])
			{
				//"libName"
				currentLibName = clear(attrStr.substr(strlen(supportAttr[0]) + 1));
				//cout << "libName is " << currentLibName << endl;

			}
			else if (attrStr.substr(0, strlen(supportAttr[1])) == supportAttr[1])
			{
				//"version"
				currentVersion = clear(attrStr.substr(strlen(supportAttr[1]) + 1));
				//cout << "version is " << currentVersion << endl;
			}
			else
			{
				printf("Unsupport attributes in line %d. \n", lineNo);
			}


		}
		else
		{
			leftPos = tempStr.find('(');
			rightPos = tempStr.find(')');
			if (leftPos != string::npos && rightPos != string::npos)
			{
				// this line is function 
				blankPos = tempStr.find(' ');

				funcType func;
				func.funcName = clear(tempStr.substr(blankPos + 1, leftPos - blankPos - 1));
				//cout << "func name is " << func.funcName << endl;

				if (currentLibName.size() < 1 || currentVersion.size() < 1)
				{
					printf("func without lib or version in line %d. \n", lineNo);
				}

				func.funcLibraryName = currentLibName;
				func.funcLibraryVersion = currentVersion;
				func.funcDedupName = string(func.funcName).append("_dedup");

				dedupConfig.funcs.push_back(std::move(func));
			}
			else
			{
				printf("brackets not match in line %d. \n", lineNo);
			}
		}
	}

	fin.close();

	return dedupConfig;
}

void replaceSourceWithDedupConfig(const dedupConfigType config, string srcFileName, string dstFileName)
{
	ifstream fin(srcFileName, ios::in);
	ofstream fout(dstFileName, ios::out | ios::trunc);

	// add dedup header file
	for (int i = 0; i < config.funcs.size(); i++)
	{
		const funcType& func = config.funcs[i];

		string libName;
		libName.append(func.funcLibraryName);
		libName.append("_dedup_v");
		libName.append(func.funcLibraryVersion);
		libName.append(".h");

		string includeStr;
		includeStr.append("#include \"");
		includeStr.append(libName);
		includeStr.append("\"\n");

		fout << includeStr;
	}

	// read source file and output
	string oneLineStr;
	int funcPos,linePos;

	while (fin.good())
	{
		getline(fin, oneLineStr);

		linePos = 0;
		for (int i = 0; i < config.funcs.size(); i++)
		{
			const funcType& func = config.funcs[i];

			funcPos = oneLineStr.find(func.funcName, linePos);
			if (funcPos != string::npos)
			{
				//tempStr = oneLineStr.substr(funcPos + func.funcName.length());
				oneLineStr.replace(funcPos, func.funcName.length(), func.funcDedupName);
				linePos = funcPos + func.funcDedupName.length();
				i--;
				continue;
			}
		}

		fout << oneLineStr << endl;
	}

	fin.close();
	fout.close();
}

string trim(const string& src)
{
	string dst;
	bool blankType = true;
	for (auto it = src.begin(); it != src.end(); it++)
	{
		if (*it == ' ' || *it == '\t')
		{
			if (!blankType)
			{
				blankType = true;
				dst.append(1, ' ');
			}
			continue;
		}
		else
		{
			blankType = false;
			dst.append(1, *it);
		}
	}
	return std::move(dst);
}

string clear(const string& src, string delim)
{
	string dst;
	bool delType = false;
	for (auto its = src.begin(); its != src.end(); its++)
	{

		for (auto itd = delim.begin(); itd != delim.end(); itd++)
		{
			if (*its == *itd)
			{
				delType = true;
				break;
			}
		}

		if (delType)
		{
			delType = false;
			continue;
		}
		dst.append(1, *its);
	}

	return std::move(dst);
}

bool beginWith(const string& src, const string& str)
{
	return src.find(str) == 0;
}












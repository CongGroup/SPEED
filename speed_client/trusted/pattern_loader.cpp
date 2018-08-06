#include "pattern_loader.h"
#include "../../common/config.h"
extern const char* snortPattern[];

void PatternLoader::load_pattern_array(PatternSet& ptnSet)
{
	ptnSet.clear();
	std::string line;
	for (int i = 0; i < pattern_count; i++)
	{
		line.assign(snortPattern[i]);
		ptnSet.push_back(ptrn_str_to_bytes(line));
	}
}


std::string PatternLoader::binary_to_string(const Binary & bin)
{
	std::string str;
	str.assign((char*)bin.data(), bin.size());
	str.append(1, '\0');
	return std::move(str);
}

char PatternLoader::cap_hex_to_byte(const std::string & hex) {
	// first half
	char byte = (hex[0] >= '0' && hex[0] <= '9') ? (hex[0] - '0') : (hex[0] - 'A' + 10); // small letters assumed
	byte *= 16;
	// second half
	byte += (hex[1] >= '0' && hex[1] <= '9') ? (hex[1] - '0') : (hex[1] - 'A' + 10);
	return byte;
}


Binary PatternLoader::ptrn_str_to_bytes(const std::string & str) {
	Binary bytes;

	size_t strlen = str.length();
	for (size_t i = 0; i < strlen; ) {
		// handle binary data in hex form
		if (str[i] == '|')
		{
			// find next '|' and extract the hex string
			size_t nextDelim = str.find('|', i + 1);
			const std::string& hexes = str.substr(i + 1, nextDelim - i - 1);

			// transform each char
			size_t idx = 0;
			while (idx < hexes.length())
			{
				if (hexes[idx] == ' ') {
					++idx;
					continue;
				}
				bytes.push_back(cap_hex_to_byte(hexes.substr(idx, 2)));
				idx += 2;
			}

			// update index
			i = nextDelim + 1;
		}
		// normal character
		else
		{
			bytes.push_back(str[i]);
			++i;
		}
	}
	return bytes;
}

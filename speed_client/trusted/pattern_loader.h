#ifndef PATTERNLOADER_H
#define PATTERNLOADER_H

#include <string>
#include <vector>

typedef uint8_t Byte;
typedef std::vector<Byte> Binary;
typedef std::vector<Binary> PatternSet;

class PatternLoader {
public:
    static void load_pattern_array(PatternSet& ptnSet);
	static std::string binary_to_string(const Binary& bin);
private:
	
    static char cap_hex_to_byte(const std::string& hex);
    static Binary ptrn_str_to_bytes(const std::string& str);
};

#endif
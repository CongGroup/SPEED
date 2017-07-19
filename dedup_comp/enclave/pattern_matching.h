#ifndef PATTERN_MATCHING_H
#define PATTERN_MATCHING_H

#include "function.h"

#include <pcre.h>

class PatternMatching : public Function
{
public:
    PatternMatching(int id, const char *regex, const char *str, int str_len);

    const byte *get_tag();

    byte *input();

    int input_size();

    byte *output();

    void process();

	void setR(const byte *r);
    
private:
    // input string for query
    std::string     m_str;

	//   name | input | r
	std::string		m_data;

    // pcre state
    pcre           *m_pcre;
    const char     *m_error;
    int             m_erroffset;

#define PCRE_OVECTOR_SIZE 1000*3 // allow at most 1000 matches
    // output matching positions in the form {s_1,e_1,s_2,e_2,...,}
    // the last n/3 of the array are reserved by libpcre and not used
    int m_ovector[PCRE_OVECTOR_SIZE];
};

#endif
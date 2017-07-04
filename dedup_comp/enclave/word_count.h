#ifndef WORD_COUNT_H
#define WORD_COUNT_H

#include "function.h"

#include <string>

class WordCount : public Function
{
public:
    WordCount(int id, const char *textfile, int filesize);

    const int get_id();

    const char *get_name();

    const int get_type();

    const byte *get_tag();

    byte *input();

    int input_size();

    byte *output();

    int expt_output_size();

    void process();

private:
    std::string     m_name;
    int             m_id;
    int             m_type;

    std::string     m_content;
    byte            m_tag[TAG_SIZE];
    
    int             m_expt_output_size; // TODO
    std::string     m_output;
};

#endif
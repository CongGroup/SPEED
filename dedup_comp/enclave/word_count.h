#ifndef WORD_COUNT_H
#define WORD_COUNT_H

#include "function.h"

#include "../../common/config.h"

#include <string>

class WordCount : public Function
{
public:
    WordCount(int id, const char *textfile, int filesize);

    const char *get_name();

    const int get_id();

    const uint8_t *get_tag();

    uint8_t *output();

    int exp_output_size();

    void process();

private:
    int             m_id;
    std::string     m_name;

    std::string     m_content;
    uint8_t         m_tag[TAG_SIZE];
    
    int             m_exp_size; // TODO
    std::string     m_output;
};

#endif
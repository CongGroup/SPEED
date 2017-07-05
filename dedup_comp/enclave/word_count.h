#ifndef WORD_COUNT_H
#define WORD_COUNT_H

#include "function.h"

class WordCount : public Function
{
public:
    WordCount(int id, const char *textfile, int filesize);

    const byte *get_tag();

    byte *input();

    int input_size();

    byte *output();

    void process();

private:
    std::string     m_input;
    std::string     m_output;
};

#endif
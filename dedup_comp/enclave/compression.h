#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "function.h"

class Compression : public Function
{
public:
    Compression(int id, const byte *file, int file_size);

    const byte *get_tag();

    byte *input();

    int input_size();

    byte *output();

    void process();

	void setR(const byte *r);

private:
	// name|input|r
	std::string		m_data;

    std::string    m_input;
    std::string    m_output;
};

#endif
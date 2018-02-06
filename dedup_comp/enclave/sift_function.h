#pragma once

#include <string>
#include "function.h"
#include "../siftpp_sgx/sift-driver.h"

class sift_function : public Function
{
public:

	sift_function(int id, const byte *pic_buffer_data, int pic_width, int pic_height);

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

	VL::PgmBuffer m_pgmBuffer;
};


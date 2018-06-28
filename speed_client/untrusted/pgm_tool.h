#pragma once

#include <iostream>




class pgm_tool
{
public:
	typedef  float pixel_t;

	struct PgmBuffer
	{
		int width;     //< Image width
		int height;    //< Image hegith
		pixel_t* data; //< Image data
	};

	
	std::ostream & insertPgm(std::ostream & os, pixel_t const * im, int width, int height);

	std::istream & extractPgm(std::istream & in, PgmBuffer & buffer);

};


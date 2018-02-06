#include "pgm_tool.h"

#include <stdio.h>
#include <stdint.h>
namespace Detail {

	/** Comment eater istream manipulator */
	class _cmnt {} cmnt;

	/** @brief Extract a comment from a stream
	**
	** The function extracts a block of consecutive comments from an
	** input stream. A comment is a sequence of whitespaces, followed by
	** a `#' character, other characters and terminated at the next line
	** ending. A block of comments is just a sequence of comments.
	**/
	std::istream&
		operator >> (std::istream& is, _cmnt& manip)
	{
		char c;
		char b[1024];
		is >> c;
		if (c != '#')
			return is.putback(c);
		is.getline(b, 1024);
		return is;
	}

}


std::ostream&
pgm_tool::insertPgm(std::ostream& os, pixel_t const* im, int width, int height)
{
	os << "P5" << "\n"
		<< width << " "
		<< height << "\n"
		<< "255" << "\n";
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			unsigned char v =
				(unsigned char)
				(std::max(std::min(*im++, 1.0f), 0.f) * 255.0f);
			os << v;
		}
	}
	return os;
}

std::istream&
pgm_tool::extractPgm(std::istream& in, PgmBuffer& buffer)
{
	pixel_t* im_pt;
	int      width;
	int      height;
	int      maxval;

	char c;
	in >> c;
	//if (c != 'P') VL_THROW("File is not in PGM format");

	bool is_ascii;
	in >> c;
	switch (c) {
	case '2': is_ascii = true; break;
	case '5': is_ascii = false; break;
	default: break;//VL_THROW("File is not in PGM format");
	}

	in >> Detail::cmnt
		>> width
		>> Detail::cmnt
		>> height
		>> Detail::cmnt
		>> maxval;

	// after maxval no more comments, just a whitespace or newline
	{char trash; in.get(trash); }

	if (maxval > 255)
		printf("Only <= 8-bit per channel PGM files are supported");

	if (!in.good())
		printf("PGM header parsing error");

	im_pt = new pixel_t[width*height];

	try {
		if (is_ascii) {
			pixel_t* start = im_pt;
			pixel_t* end = start + width*height;
			pixel_t  norm = pixel_t(maxval);

			while (start != end) {
				int i;
				in >> i;

				*start++ = pixel_t(i) / norm;
			}
		}
		else {
			std::streampos beg = in.tellg();
			char* buffer = new char[width*height];
			in.read(buffer, width*height);


			pixel_t* start = im_pt;
			pixel_t* end = start + width*height;
			uint8_t* src = reinterpret_cast<uint8_t*>(buffer);
			while (start != end) *start++ = *src++ / 255.0f;
		}
	}
	catch (...) {
		delete[] im_pt;
		throw;
	}

	buffer.width = width;
	buffer.height = height;
	buffer.data = im_pt;

	return in;
}

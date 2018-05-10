#include "sift_function.h"



#include "crypto.h"
#include "sysutils.h"

#include <cstring>

using namespace std;

sift_function::sift_function(int id, const byte *pic_buffer_data, int pic_width, int pic_height)
	: Function(id, FUNC_SF, "Picture Feature"),
	m_input((char*)pic_buffer_data, pic_width*pic_height* sizeof(float)),
	m_output(pic_width*pic_height * sizeof(float)*4, 0)
{
	m_data.reserve(m_name.size() + RAND_SIZE + m_input.size());
	m_data.assign(m_name);
	m_data.append(m_input.data(), m_input.data() + m_input.size());

	m_pgmBuffer.width = pic_width;
	m_pgmBuffer.height = pic_height;
	m_pgmBuffer.data = (float*)m_input.data();

	m_expt_output_size = m_output.size();
}

static void sift_printBlock(byte* p, size_t size)
{
	for (int i = 0; i < size; i++)
	{
		eprintf("%.2X ", p[i]);
	}
	eprintf("\n");
}

void sift_function::process()
{
	//sift_printBlock((byte*)m_pgmBuffer.data, 128);

	const int   O = -1;
	const int   S = 3;
	const int   omin = -1;
	const float sigman = 0.5f;
	const float sigma0 = 2.0f;
	const int	smin = -1;
	const int	smax = 4;
	m_output = computeSift(m_pgmBuffer.data, m_pgmBuffer.width, m_pgmBuffer.height,
		sigman, sigma0, O, S, omin, smin, smax);
	m_expt_output_size = m_output.size();
}

byte *sift_function::input()
{
	return (byte *)m_data.data();
}

int sift_function::input_size()
{
	return m_data.size();
}

byte *sift_function::output()
{
	return (byte *)m_output.data();
}

const byte *sift_function::get_tag()
{
	::hash((byte *)(m_data.data()), m_name.size() + m_input.size(), m_tag);
	return m_tag;
}

void sift_function::setR(const byte * r)
{
	m_data.append((const char*)r, (const char*)r + RAND_SIZE);
}

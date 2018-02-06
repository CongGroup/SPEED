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

void sift_function::process()
{
	m_output = computeDefaultSift(m_pgmBuffer);
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

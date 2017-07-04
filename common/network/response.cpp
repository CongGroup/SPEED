#include "response.h"

const byte Response::Positive = 1;
const byte Response::Negative = 2;

Response::Response(const byte * data, int size)
    : m_data (data),
      m_size (size)
{
}

const byte * Response::get_data() const
{
    return m_data;
}

int Response::get_size() const
{
    return m_size;
}

const byte Response::get_answer() const
{
    // parse first byte as answer
    return m_data[0];
}

const byte * Response::get_meta() const
{
    // metadata starts from the second byte, if any
    if (m_size > 1)
        return &m_data[1];
    else
        return 0;
}

const byte * Response::get_rlt() const
{
    // result data follows metadata, if any
    if (m_size > 1)
        return &m_data[1 + sizeof(metadata)];
    else
        return 0;
}

int Response::get_rlt_size() const
{
    // Positive
    if (m_size > 1)
        return m_size - 1 - sizeof(metadata);
    // Negative
    else
        return 0;
}

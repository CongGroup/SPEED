#include "request.h"
#include <string.h>

Request::Request(const uint8_t * data, int size)
    : m_data (data),
      m_size (size)
{
}

const uint8_t * Request::get_data() const
{
    return m_data;
}

int Request::get_size() const
{
    return m_size;
}

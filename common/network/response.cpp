#include "response.h"

#include "../data_type.h"

Response::Response(const uint8_t * data, int raw_size)
    : m_data (data),
      m_size (raw_size)
{
}

const uint8_t * Response::get_data() const
{
    return m_data;
}

int Response::get_size() const
{
    return m_size;
}

const uint8_t * Response::get_meta() const
{
    return m_data;
}

const uint8_t * Response::get_rlt() const
{
    return m_data + sizeof(metadata);
}

int Response::get_rlt_size() const
{
    return m_size - sizeof(metadata);
}


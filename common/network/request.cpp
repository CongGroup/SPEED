#include "request.h"

const byte Request::Get = 1;
const byte Request::Put = 2;

Request::Request(const byte * data, int size)
    : m_data      (data),
      m_size      (size),
      m_rlt_size  (size- REQ_TYPE_SIZE - TAG_SIZE - sizeof(metadata)),
      m_expt_rlt_size ( 0 )
{
    if (get_type() == Get)
        m_expt_rlt_size = *((int *)(&m_data[REQ_TYPE_SIZE + TAG_SIZE]));
}

const byte * Request::get_data() const
{
    return m_data;
}

int Request::get_size() const
{
    return m_size;

}

byte Request::get_type() const
{
    // parse first byte as type
    return m_data[0];
}

const byte *Request::get_tag() const
{
    // tag starts from the second byte
    return &m_data[0 + REQ_TYPE_SIZE];
}

const byte *Request::get_meta() const
{
    // metadata follows tag, if any
    if (m_rlt_size > 0)
        return &m_data[REQ_TYPE_SIZE + TAG_SIZE];
    else
        return 0;
}

const byte *Request::get_rlt() const
{
    // computation result follows metadata, if any
    if (m_rlt_size > 0)
        return &m_data[1 + TAG_SIZE + sizeof(metadata)];
    else
        return 0;
}

int Request::get_expt_rlt_size() const
{
    return m_expt_rlt_size;
}

int Request::get_rlt_size() const 
{
    return m_rlt_size;
}
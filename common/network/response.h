#ifndef RESPONSE_H
#define RESPONSE_H

#include "../config.h"

#include <stdint.h>

// This class is for parsing purpose, holding no actual data
// currently just one type of response for "find request"
class Response
{
public:
    Response(const uint8_t *data, int size);

    const uint8_t *get_data() const;

    int get_size() const;

    const uint8_t *get_meta() const;

    const uint8_t *get_rlt() const;
    
    int get_rlt_size() const;

private:
    const uint8_t *m_data;
    int            m_size;
};

#endif


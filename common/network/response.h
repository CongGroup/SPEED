#ifndef RESPONSE_H
#define RESPONSE_H

#include "../config.h"

#include <stdint.h>

// This class is for parsing purpose, holding no actual data
// currently just one type of response for "find request"
class Response
{
public:
    static const uint8_t Positive;
    static const uint8_t Negative;

public:
    Response(const uint8_t *data, int size);

    // raw data
    const uint8_t *get_data() const;
    int get_size() const;

    const uint8_t get_answer() const;
    
    // for positive response (of Get request)
    const uint8_t *get_meta() const;
    const uint8_t *get_rlt() const;
    int get_rlt_size() const;

private:
    const uint8_t *m_data;
    int            m_size;
};

#endif


#ifndef REQUEST_H
#define REQUEST_H

#include "../config.h"

// Find request
// This class is for parsing purpose, holding no actual data
class Request
{
public:
    static const uint8_t Get;
    static const uint8_t Put;

public:
    Request(const uint8_t* data, int size);

    // raw data
    const uint8_t* get_data() const;
    int get_size() const;

    // for both types
    uint8_t get_type() const;
    const uint8_t *get_tag() const;

    // for Get request
    int get_expt_rlt_size() const;

    // for Put request
    const uint8_t *get_meta() const;
    const uint8_t *get_rlt() const;
    int get_rlt_size() const;

private:
    const uint8_t* m_data;
	int            m_size;
    // Get
    int            m_expt_rlt_size;
    // Put
    int            m_rlt_size;
};

#endif

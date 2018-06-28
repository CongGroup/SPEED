#ifndef REQUEST_H
#define REQUEST_H

#include "../data_type.h"

// Find request
// This class is for parsing purpose, holding no actual data
class Request
{
public:
    static const byte Get;
    static const byte Put;

public:
    Request(const byte* data, int size);

    // raw data
    const byte* get_data() const;
    int get_size() const;

    // for both types
    byte get_type() const;
    const byte *get_tag() const;

    // for Get request
    int get_expt_rlt_size() const;

    // for Put request
    const byte *get_meta() const;
    const byte *get_rlt() const;
    int get_rlt_size() const;

private:
    const byte* m_data;
	int            m_size;
    // Get
    int            m_expt_rlt_size;
    // Put
    int            m_rlt_size;
};

#endif

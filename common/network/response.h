#ifndef RESPONSE_H
#define RESPONSE_H

#include "../data_type.h"

// This class is for parsing purpose, holding no actual data
// currently just one type of response for "find request"
class Response
{
public:
	// sign the server have data or not
    static const byte Positive;
    static const byte Negative;

public:
    Response(const byte *data, int size);

    // raw data
    const byte *get_data() const;
    int get_size() const;

    const byte get_answer() const;
    
    // for positive response (of Get request)
    const byte *get_meta() const;
    const byte *get_rlt() const;
    int get_rlt_size() const;

private:
    const byte *m_data;
    int   m_size;
};

#endif


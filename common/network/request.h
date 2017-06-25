#ifndef REQUEST_H
#define REQUEST_H

#include "../config.h"

#include <stdint.h>
#include <netinet/in.h>

// Find request
// This class is for parsing purpose, holding no actual data
class Request
{
public:
	Request(const uint8_t* data, int size);

	const uint8_t* get_data() const;
	int get_size() const;

private:
    const uint8_t* m_data;
	int            m_size;
};

class PutRequest : public Request
{
public:
    PutRequest(const uint8_t* data, int size) 
        : Request(data, size) {}
};

#endif

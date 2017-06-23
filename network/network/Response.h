#pragma once
#include <stdint.h>
class Response
{
public:
	Response();
	Response(char* data, uint32_t size);
	~Response();

	char* getData() const;
	uint32_t getSize() const;

	void set(char* data, uint32_t size);

private:
	char* data;
	uint32_t size;
};


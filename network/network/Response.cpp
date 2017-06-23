#include "Response.h"
#include <string.h>

Response::Response()
{
	size = 0;
	data = 0;
}

Response::Response(char* data, uint32_t size)
{
	this->size = size;
	this->data = new char[size];

	memcpy(this->data, data, size);
}


Response::~Response()
{
	if (data)
	{
		delete[] data;
	}
}

char* Response::getData() const
{
	return data;
}

uint32_t Response::getSize() const
{
	return size;
}

void Response::set(char * data, uint32_t size)
{
	if (this->data)
	{
		delete[] this->data;
	}

	this->size = size;
	this->data = new char[size];

	memcpy(this->data, data, size);
}

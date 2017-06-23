#include "Request.h"
#include <string.h>

Request::Request()
{
	size = 0;
	data = 0;
}

Request::Request(char* data, uint32_t size)
{
	this->size = size;
	this->data = new char[size];
	
	memcpy(this->data, data, size);
}


Request::~Request()
{
	if (data)
	{
		delete[] data;
	}
}

char* Request::getData() const
{
	return data;
}

uint32_t Request::getSize() const
{
	return size;
}

sockaddr_in Request::getAddr() const
{
	return fromAddr;
}

void Request::set(char * data, uint32_t size)
{
	if (this->data)
	{
		delete[] this->data;
	}

	this->size = size;
	this->data = new char[size];

	memcpy(this->data, data, size);
}

void Request::setAddr(const sockaddr_in& from)
{
	fromAddr = from;
}

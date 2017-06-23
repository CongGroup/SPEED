#pragma once
#include <stdint.h>
#include <netinet/in.h>
class Request
{
public:
	Request();
	Request(char* data, uint32_t size);
	~Request();

	char* getData() const;
	uint32_t getSize() const;
	sockaddr_in getAddr() const;
	int getClientfd() const;

	void set(char* data, uint32_t size);
	void setAddr(const sockaddr_in& from);
	void setClientfd(int clientfd);


private:
	char* data;
	uint32_t size;

	int clientfd;
	sockaddr_in fromAddr;
};


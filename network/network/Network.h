#pragma once


#include "Request.h"
#include "Response.h"

class Network
{
public:
	int sendRequest(char* ip, int port, Request& request);
	int recvRequest(Request* request);

	int sendRespons(const Request& request, Response& response);
	int recvRespons(int sockid, Response* response);

protected:
	int newUDPSocket();
	sockaddr_in newAddr(char * ip, int port);
	int bindSocket(int sid, char* ip, int port);
	int recvMsg(int sid, sockaddr_in* from, char** data, uint32_t* dataSize);
};


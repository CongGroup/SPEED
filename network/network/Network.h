#pragma once


#include "Request.h"
#include "Response.h"

class Network
{
public:
	//Init Network tools
	//If ServerIP == null the tools will use in server side.
	//The port is always need.
	Network(char* ServerIP, int port, bool useTCP = false);
	~Network();

	/*
	 * send request to server
	 */
	int sendRequest(const Request* request);

	/*
	 * recv msg from server
	 */
	int recvRespons(Response* response);

	/*
	 * recv msg from client
	 */
	int recvRequest(Request* request);

	/*
	 * send response for specific request
	 */
	int sendRespons(const Request* request, const Response* response);


protected:
	int newUDPSocket();
	sockaddr_in newAddr(char * ip, int port);
	int bindSocket(int sid, char* ip, int port);
	int sendMsg(int sid, sockaddr_in * to, const char * data, const uint32_t dataSize);
	int recvMsg(int sid, sockaddr_in * from, char ** pdata, uint32_t * psize);

public:
	int newTCPSocket();
	int accept(int sid, sockaddr_in * from);
	int sendMsg(int sid, const char * data, const uint32_t dataSize);
	int recvMsg(int sid, char** data, uint32_t* dataSize);

protected:
	int sockfd;						//socket ID
	sockaddr_in serv_addr;			//server addr

	bool isTCP;
};


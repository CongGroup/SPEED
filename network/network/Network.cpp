#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "Network.h"
#include "config.h"

//#include <iostream>
//using namespace std;

Network::Network(char * ServerIP, int port, bool useTCP)
{
	this->isTCP = useTCP;

	if (useTCP)
	{
		sockfd = newTCPSocket();
		if (ServerIP)
		{
			close(sockfd);
		}
	}
	else
	{
		sockfd = newUDPSocket();
	}
	
	
	if (ServerIP)
	{
		serv_addr = newAddr(ServerIP, port);
	}
	else
	{
		bindSocket(sockfd, 0, port); //bind server
		if (useTCP)
		{
			const int ListSize = 20;
			listen(sockfd, ListSize);
		}
	}
}

Network::~Network()
{
	close(sockfd);
}

int Network::sendRequest(const Request* request)
{
	if (isTCP)
	{
		sockfd = newTCPSocket();
		connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
		sendMsg(sockfd, request->getData(), request->getSize());
	}
	else
	{
		sendMsg(sockfd, &serv_addr, request->getData(), request->getSize());
	}
	return 0;
}

int Network::recvRequest(Request * request)
{
	sockaddr_in client_addr; //client address

	char* data;
	uint32_t dataSize;

	if (isTCP)
	{
		int clientfd = accept(sockfd, &client_addr);
		recvMsg(clientfd, &data, &dataSize);
		request->setClientfd(clientfd);
	}
	else
	{
		recvMsg(sockfd, &client_addr, &data, &dataSize);
		request->setAddr(client_addr);
	}
	request->set(data, dataSize);

	delete[] data;
	return 0;
}

int Network::sendRespons(const Request* request, const Response* response)
{
	int clientfd;						//socket ID

	if (isTCP)
	{
		clientfd = request->getClientfd();
		sendMsg(clientfd, response->getData(), response->getSize());
	}
	else
	{
		clientfd = newUDPSocket();

		sockaddr_in client_addr = request->getAddr();
		sendMsg(clientfd, &client_addr, response->getData(), response->getSize());
	}

	close(clientfd);
	return 0;
}

int Network::recvRespons(Response * response)
{
	char* data;
	uint32_t dataSize;

	if (isTCP)
	{
		recvMsg(sockfd, &data, &dataSize);
		close(sockfd);
	}
	else
	{
		recvMsg(sockfd, 0, &data, &dataSize);
	}


	response->set(data, dataSize);

	delete[] data;
	return 0;
}

int Network::newUDPSocket()
{
	return socket(AF_INET, SOCK_DGRAM, 0);  //TCP/IP-UPD
}

int Network::newTCPSocket()
{
	return socket(AF_INET, SOCK_STREAM, 0);
}

int Network::accept(int sid, sockaddr_in * from)
{
	socklen_t length = sizeof(sockaddr);
	return ::accept(sid, (sockaddr*)from, &length);
}

int Network::sendMsg(int sid, const char * data, const uint32_t dataSize)
{
	send(sid, &dataSize, sizeof(uint32_t), 0);
	//cout << "S1 " << dataSize << endl;
	//sleep(1);
	send(sid, data, dataSize, 0);
	//cout << "S2 " << data << endl;
	return 0;
}

int Network::recvMsg(int sid, char ** pdata, uint32_t * psize)
{
	char sizebuf[sizeof(uint32_t)] = { 0 };
	recv(sid, sizebuf, sizeof(sizebuf), 0);
	uint32_t dataSize = *(uint32_t*)sizebuf;

	//cout << "R1 " << dataSize << endl;

	char* databuf = new char[dataSize];
	memset(databuf, 0, dataSize);
	recv(sid, databuf, dataSize, 0);

	//cout << "R2 " << databuf << endl;

	*pdata = databuf;
	*psize = dataSize;

	return 0;
}

sockaddr_in Network::newAddr(char * ip, int port)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;							//IPV4
	addr.sin_port = htons(port);						//Port
	inet_aton(ip, (in_addr *)&addr.sin_addr.s_addr);	//setIP
	return addr;
}

int Network::bindSocket(int sid, char * ip, int port)
{
	sockaddr_in host_addr;						//address
	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.sin_family = AF_INET;             //TCP/IP
	host_addr.sin_port = htons(port);			//set port
	if (ip)
	{
		inet_aton(ip, (in_addr *)&host_addr.sin_addr.s_addr);
	}
	else
	{
		host_addr.sin_addr.s_addr = INADDR_ANY;     //server IP
	}
	return bind(sid, (sockaddr *)&host_addr, sizeof(host_addr)); //bind server
}

int Network::sendMsg(int sid, sockaddr_in * to, const char * data, const uint32_t dataSize)
{
	sendto(sid, &dataSize, sizeof(uint32_t), 0, (sockaddr *)to, sizeof(sockaddr));
	sendto(sid, data, dataSize, 0, (sockaddr *)to, sizeof(sockaddr));
	return 0;
}

int Network::recvMsg(int sid, sockaddr_in * from, char ** pdata, uint32_t * psize)
{
	socklen_t len = from ? sizeof(*from) : 0;

	char sizebuf[sizeof(uint32_t)] = { 0 };
	recvfrom(sid, sizebuf, sizeof(sizebuf), 0, (sockaddr *)from, &len);
	uint32_t dataSize = *(uint32_t*)sizebuf;

	char* databuf = new char[dataSize];
	memset(databuf, 0, dataSize);
	recvfrom(sid, databuf, dataSize, 0, (sockaddr *)from, &len);

	*pdata = databuf;
	*psize = dataSize;

	return 0;
}

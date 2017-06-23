#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "Network.h"
#include "config.h"

Network::Network(char * ServerIP, int port)
{
	sockfd = newUDPSocket();
	
	if (ServerIP)
	{
		serv_addr = newAddr(ServerIP, port);
	}
	else
	{
		bindSocket(sockfd, 0, port); //bind server
	}
}

Network::~Network()
{
	close(sockfd);
}

int Network::sendRequest(const Request* request)
{
	sendMsg(sockfd, &serv_addr, request->getData(), request->getSize());
	return 0;
}


int Network::recvRequest(Request * request)
{
	sockaddr_in client_addr; //client address

	char* data;
	uint32_t dataSize;
	recvMsg(sockfd, &client_addr, &data, &dataSize);

	request->set(data, dataSize);
	request->setAddr(client_addr);
	
	delete[] data;
	return 0;
}


int Network::sendRespons(const Request* request, const Response* response)
{
	int sockfdtoClient;						//socket ID   
	sockfdtoClient = newUDPSocket();

	sockaddr_in client_addr = request->getAddr();

	sendMsg(sockfdtoClient, &client_addr, response->getData(), response->getSize());

	close(sockfdtoClient);
	return 0;
}

int Network::recvRespons(Response * response)
{
	char* data;
	uint32_t dataSize;
	recvMsg(sockfd, 0, &data, &dataSize);

	response->set(data, dataSize);

	delete[] data;
	return 0;
}

int Network::newUDPSocket()
{
	return socket(AF_INET, SOCK_DGRAM, 0);  //TCP/IP-UPD
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

	char sizebuf[64] = { 0 };
	recvfrom(sid, sizebuf, sizeof(sizebuf), 0, (struct sockaddr *)from, &len);
	uint32_t dataSize = *(uint32_t*)sizebuf;

	char* databuf = new char[dataSize];
	memset(databuf, 0, dataSize);
	recvfrom(sid, databuf, dataSize, 0, (struct sockaddr *)from, &len);

	*pdata = databuf;
	*psize = dataSize;

	return 0;
}

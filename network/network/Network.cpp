#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "Network.h"
#include "config.h"


/*
 * send request to server
 * return socketID if success
 * return -1 if socket init error
 */
int Network::sendRequest(char * ip, int port, Request & request)
{
	int sockfd;						//socket ID
	sockaddr_in serv_addr;   //server addr

	sockfd = newUDPSocket();
	if (sockfd == -1)
	{
		return -1;
	}
	
	serv_addr = newAddr(ip, port);

	uint32_t dataSize = request.getSize();
	sendto(sockfd, &dataSize, sizeof(dataSize), 0, (sockaddr *)&serv_addr, sizeof(serv_addr));
	sendto(sockfd, request.getData(), request.getSize(), 0, (sockaddr *)&serv_addr, sizeof(serv_addr));

	return sockfd;
}

/*
* recv msg from server port then close it
* return  0 if success
* return -1 if socket init error
* return -2 if bind error
*/
int Network::recvRequest(Request * request)
{
	int sockid;				 //socketID
	sockaddr_in host_addr;   //server address
	sockaddr_in client_addr; //client address

	sockid = newUDPSocket();  //TCP/IP-UPD
	if (sockid == -1)
	{
		return -1;
	}

	int ret = bindSocket(sockid, "127.0.0.1", SERV_PORT); //bind server
	if (ret == -1)
	{
		return -2;
	}

	char* data;
	uint32_t dataSize;
	recvMsg(sockid, &client_addr, &data, &dataSize);

	request->set(data, dataSize);
	request->setAddr(client_addr);
	
	delete[] data;
	close(sockid);   //close
	return 0;
}

/*
 * use per request to send response
 * return  0 if success
 * return -1 if socket init error
 */
int Network::sendRespons(const Request& request, Response & response)
{
	int sockfd;						//socket ID   

	sockfd = newUDPSocket();
	if (sockfd == -1)
	{
		return -1;
	}

	sockaddr_in client_addr = request.getAddr();

	uint32_t dataSize = response.getSize();
	sendto(sockfd, &dataSize, sizeof(dataSize), 0, (sockaddr *)&client_addr, sizeof(client_addr));
	sendto(sockfd, response.getData(), response.getSize(), 0, (sockaddr *)&client_addr, sizeof(client_addr));

	close(sockfd);
	return 0;
}

/*
 * use request sock to recv server response
 * return  0 if success
 */
int Network::recvRespons(int sockid, Response * response)
{
	char* data;
	uint32_t dataSize;
	recvMsg(sockid, 0, &data, &dataSize);

	response->set(data, dataSize);
	
	delete[] data;
	close(sockid);
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
	sockaddr_in host_addr;				//address
	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.sin_family = AF_INET;             //TCP/IP
	host_addr.sin_port = htons(SERV_PORT);      //set port
	//host_addr.sin_addr.s_addr = INADDR_ANY;     //server IP
	inet_aton(ip, (in_addr *)&host_addr.sin_addr.s_addr);
	return bind(sid, (struct sockaddr *)&host_addr, sizeof host_addr); //bind server
}

int Network::recvMsg(int sid, sockaddr_in * from, char ** pdata, uint32_t * size)
{
	socklen_t len = from ? sizeof(*from) : 0;

	char sizebuf[64] = { 0 };
	recvfrom(sid, sizebuf, sizeof(sizebuf), 0, (struct sockaddr *)from, &len);
	uint32_t dataSize = *(uint32_t*)sizebuf;

	char* databuf = new char[dataSize];
	memset(databuf, 0, dataSize);
	recvfrom(sid, databuf, dataSize, 0, (struct sockaddr *)from, &len);

	if (pdata)
	{
		*pdata = databuf;
		*size = dataSize;
	}

	return 0;
}

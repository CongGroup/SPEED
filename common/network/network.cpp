#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "network.h"
#include "../config.h"

#define SEND_FLAG 0
#define RECV_FLAG 0
#define BACKLOG 20

Network::Network(char * server_ip)
    : requester (0),
      responser (0)
{
    // client
    if (server_ip) {
        serv_addr = new_addr(server_ip);
        requester = socket(AF_INET, SOCK_STREAM, 0);

        // TODO error handling
        connect(requester, (sockaddr *)&serv_addr, sizeof(serv_addr));
    }
    // server
    else {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);

        bind_socket(sockfd, 0);

        listen(sockfd, BACKLOG);

        // TODO error handling
        socklen_t len = sizeof(client_addr);
        responser = accept(sockfd, (sockaddr *)&client_addr, &len);
	}
}

Network::~Network()
{
    if(requester)
        close(requester);
    if(responser)
	    close(responser);
}

int Network::send_request(const Request* request)
{
	send_msg(requester, request->get_data(), request->get_size());
	return 0;
}

Request *Network::recv_request()
{
	int size = recv_msg(responser);
    if (size > 0) {
        Request* req = new Request(m_recv_buffer, size);
        return req;
    }
    else {
        return nullptr;
    }
}

int Network::send_response(const Response* response)
{
	send_msg(responser, response->get_data(), response->get_size());
	return 0;
}

Response * Network::recv_response()
{
    int resp_size = recv_msg(requester);
    if (resp_size > 0) {
        Response* resp = new Response(m_recv_buffer, resp_size);
        return resp;
    }
    else {
        return nullptr;
    }
}

void Network::set_send_buffer(const uint8_t * data, int size, int pos)
{
    if ((pos + size -1) <= TX_BUFFER_SIZE) {
        memcpy(m_send_buffer + pos, data, size);
    }
}

const uint8_t *Network::get_send_buffer() const
{
    return m_send_buffer;
}

int Network::send_msg(int sockfd, const uint8_t *data, int data_size)
{
	send(sockfd, &data_size, sizeof(int), SEND_FLAG);
	send(sockfd, data, data_size, SEND_FLAG);
	return 0;
}

int Network::recv_msg(int sockfd)
{
    ssize_t recv_size = 0;
    recv_size = recv(sockfd, m_recv_buffer, sizeof(ssize_t), RECV_FLAG);
    if (recv_size > 0) {
        recv_size = recv(sockfd, m_recv_buffer, recv_size, RECV_FLAG);
        if (recv_size > 0)
            return recv_size;
    }
    return 0;
}

sockaddr_in Network::new_addr(char * ip)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;							//IPV4
	addr.sin_port = htons(SERV_PORT);					//Port
	inet_aton(ip, (in_addr *)&addr.sin_addr.s_addr);	//setIP
	return addr;
}

int Network::bind_socket(int sid, char * ip)
{
	sockaddr_in host_addr;						//address
	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.sin_family = AF_INET;             //TCP/IP
	host_addr.sin_port = htons(SERV_PORT);	    //set port
	if (ip) {
		inet_aton(ip, (in_addr *)&host_addr.sin_addr.s_addr);
	}
	else {
		host_addr.sin_addr.s_addr = INADDR_ANY;     //server IP
	}
	return bind(sid, (sockaddr *)&host_addr, sizeof(host_addr)); //bind server
}


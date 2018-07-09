#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

#include "network.h"

#define SEND_FLAG 0
#define RECV_FLAG 0
#define BACKLOG 20

Network::Network(char * server_ip)
    : requester (0),
      responser (0),
	server_socket(0)
{

	m_send_buffer = new byte[TX_BUFFER_SIZE];
	m_recv_buffer = new byte[RX_BUFFER_SIZE];

	

    // client
    if (server_ip) {
        serv_addr = new_addr(server_ip);
        requester = socket(AF_INET, SOCK_STREAM, 0);

        // TODO error handling
		if (connect(requester, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			printf("connect server %s error.", server_ip);
    }
    // server
    else {
		server_socket = socket(AF_INET, SOCK_STREAM, 0);

        bind_socket(server_socket, 0);

        listen(server_socket, BACKLOG);

        // TODO error handling
        socklen_t len = sizeof(client_addr);
        responser = accept(server_socket, (sockaddr *)&client_addr, &len);
		if (responser < 0)
		{
			printf("accept client error %d.", responser);
		}
	}
}

Network::~Network()
{
    if(requester)
        close(requester);
    if(responser)
	    close(responser);
	if (server_socket)
		close(server_socket);

	delete[] m_send_buffer;
	delete[] m_recv_buffer;
}

static void get_hrtime(hrtime *time)
{
	timespec wall_clock;
	clock_gettime(CLOCK_REALTIME, &wall_clock);
	time->second = wall_clock.tv_sec;
	time->nanosecond = wall_clock.tv_nsec;
}

static int diff_hrtime(const hrtime *start, const hrtime *end) {
	return (end->second - start->second) * 1000000.0 +
		(end->nanosecond - start->nanosecond) / 1000.0;
}

int Network::send_request(const Request* request)
{
	hrtime start_time, end_time;
	get_hrtime(&start_time);
	send_msg(requester, request->get_data(), request->get_size());
	get_hrtime(&end_time);
	last_send_request_time = diff_hrtime(&start_time, &end_time);
	return 0;
}

Request *Network::recv_request()
{
	hrtime start_time, end_time;
	get_hrtime(&start_time);

	int size = recv_msg(responser);
	Request* req;
    if (size > 0) {
		req = new Request(m_recv_buffer, size);
    }
    else {
		req =  NULL;
    }

	get_hrtime(&end_time);
	last_recv_request_time = diff_hrtime(&start_time, &end_time);

	return req;
}

int Network::send_response(const Response* response)
{
	hrtime start_time, end_time;
	get_hrtime(&start_time);

	send_msg(responser, response->get_data(), response->get_size());

	get_hrtime(&end_time);
	last_send_response_time = diff_hrtime(&start_time, &end_time);
	return 0;
}

Response * Network::recv_response()
{
	hrtime start_time, end_time;
	get_hrtime(&start_time);

	Response* resp;
    int resp_size = recv_msg(requester);
    if (resp_size > 0) {
        resp = new Response(m_recv_buffer, resp_size);
    }
    else {
		resp = NULL;

    }

	get_hrtime(&end_time);
	last_recv_response_time = diff_hrtime(&start_time, &end_time);

	return resp;
}

void Network::reset()
{
	if (responser)
		close(responser);

	socklen_t len = sizeof(client_addr);
	responser = accept(server_socket, (sockaddr *)&client_addr, &len);
	if (responser < 0)
	{
		printf("accept client error %d.", responser);
	}
}

void Network::set_send_buffer(const byte * data, int size, int pos)
{
    if ((pos + size -1) <= TX_BUFFER_SIZE) {
        memcpy(m_send_buffer + pos, data, size);
    }
}

byte *Network::get_send_buffer()
{
    return m_send_buffer;
}

int Network::send_msg(int sockfd, const byte *data, int data_size)
{
    int size = data_size;
	//printf("send msg size , size is %d \n", data_size);
	send(sockfd, &size, sizeof(int), SEND_FLAG);
	int complete_size = 0;
	while (size>0)
	{
		complete_size = send(sockfd, data + (data_size-size), size, SEND_FLAG);
		size -= complete_size;
	}

	return 0;
}

int Network::recv_msg(int sockfd)
{
    int recv_size = 0;
    recv(sockfd, &recv_size, sizeof(int), RECV_FLAG);
	//printf("resv msg size , size is %d \n",recv_size);
    if (recv_size > 0) {
		int msg_size = recv_size;
		int complete_size = 0;
		while (msg_size > 0)
		{
			complete_size = recv(sockfd, m_recv_buffer+recv_size-msg_size, msg_size, RECV_FLAG);
			msg_size -= complete_size;
		}
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


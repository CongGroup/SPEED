#ifndef NETWOKR_H
#define NETWOKR_H

#include "request.h"
#include "response.h"

#include <netinet/ip.h>

/*
 * Currently we underlay our service with TCP.
 * Interfacize this if other protocols are to be included.
 * We maintain persistent TCP connection between caching and
 * computing server for performance.
 */
class Network
{
public:
	/*
     * Init Network tools
	 * If ServerIP == null the tools will use in server side.
	 * The port is always need.
     */
	Network(char* server_ip);
	~Network();

	/*
	 * caller
     * client ---> server
	 */
	int send_request(const Request* request);

    /*
     *             caller
     * client ---> server
     */
    Request *recv_request();

    /*
     *             caller
     * client <--- server
     */
    int send_response(const Response* response);

	/*
	 * caller
     * client <--- server
	 */
    Response *recv_response();

    /*
     * buffer accesser
     */
    void set_send_buffer(const uint8_t *data, int size, int pos);
    uint8_t* get_send_buffer();

//private:
	sockaddr_in new_addr(char * ip);
	int bind_socket(int sockfd, char* ip);
	int send_msg(int sockfd, const uint8_t *data, int data_size);
	int recv_msg(int sockfd);

//private:
	int requester;	
    sockaddr_in client_addr;
    
    int responser;
    sockaddr_in serv_addr;

    uint8_t m_send_buffer[TX_BUFFER_SIZE];
    uint8_t m_recv_buffer[RX_BUFFER_SIZE];
};

#endif
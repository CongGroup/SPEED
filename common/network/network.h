#ifndef NETWOKR_H
#define NETWOKR_H

#include "request.h"
#include "response.h"

#include "../data_type.h"

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
     * caller
     * client <--- server
     */
    int send_response(const Response* response);

	/*
	 * caller
     * client <--- server
	 */
    Response *recv_response();

	/*
	* caller
	* server
	*/
	void reset();


    /*
     * buffer accesser
     */
    void set_send_buffer(const byte *data, int size, int pos);
    byte* get_send_buffer();

//private:
	sockaddr_in new_addr(char * ip);
	int bind_socket(int sockfd, char* ip);
	int send_msg(int sockfd, const byte *data, int data_size);
	int recv_msg(int sockfd);

//private:
	int requester;	
    sockaddr_in client_addr;
    
    int responser;
    sockaddr_in serv_addr;

	int server_socket;

    byte* m_send_buffer;
    byte* m_recv_buffer;

//private:

	int last_send_request_time;
	int last_recv_request_time;
	int last_send_response_time;
	int last_recv_response_time;
};

#endif
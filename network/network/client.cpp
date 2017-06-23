#include <stdio.h>
#include <string.h>

#include "Network.h"
#include "config.h"



int main(int argc, char *argv[])
{
	Network net;

	char* str = "Hello server.";
	Request req(str, strlen(str) + 1);
	int sendid = net.sendRequest("127.0.0.1", SERV_PORT, req);
	Response res;
	net.recvRespons(sendid, &res);
	printf("%d :%s\n", res.getSize(), res.getData());
	return 0;
}

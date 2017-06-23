#include <stdio.h>
#include <string.h>

#include "Network.h"
#include "config.h"



int main(int argc, char *argv[])
{
	//init network
	Network net("127.0.0.1",SERV_PORT);

	//send request
	char* str = "Hello server.";
	Request req(str, strlen(str) + 1);
	net.sendRequest(&req);

	//recv response
	Response res;
	net.recvRespons(&res);

	//check msg
	printf("%d :%s\n", res.getSize(), res.getData());

	return 0;
}

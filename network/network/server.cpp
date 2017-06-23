#include <stdio.h>
#include <string.h>

#include "Network.h"
#include "config.h"


int main(int argc, char *argv[])
{
	//init network
	Network net(0,SERV_PORT);

	//init data
	char* str = "Hello client.";
	Request request;

	while (true)
	{
		//recv request
		net.recvRequest(&request);

		//check msg
		printf("%d :%s\n", request.getSize(),request.getData());

		//send response
		Response res(str, strlen(str) + 1);
		net.sendRespons(&request, &res);
	}
	return 0;
}

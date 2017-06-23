#include <stdio.h>
#include <string.h>

#include "Network.h"
#include "config.h"


int main(int argc, char *argv[])
{
	Network net;
	char* str = "Hello client.";
	Request request;
	while (true)
	{
		net.recvRequest(&request);
		printf("%d :%s\n", request.getSize(),request.getData());

		Response res(str, strlen(str) + 1);
		net.sendRespons(request, res);
	}
	return 0;
}

//#include <stdio.h>
//#include <string.h>
//
//#include "network.h"
//
//int main(int argc, char *argv[])
//{
//	Network* net = new Network(0);
//	char* str = "Hello client.";
//
//	while (true)
//	{
//		Request * req = net->recv_request();
//
//		if (req != NULL)
//		{
//			printf("Get data and the size is %d\n",  req->get_size());
//		}
//		else
//		{
//			printf("null request.\n");
//			break;
//		}
//
//		Response rep(str, strlen(str) + 1);
//
//		net->send_response(&rep);
//	}
//	return 0;
//}

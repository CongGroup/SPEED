//#include <stdio.h>
//#include <string.h>
//#include <unistd.h>
//#include <fstream>
//#include <sstream>
//
//#include "network.h"
//#include "../config.h"
//
//void load_file(const char *filename, char *buffer, int buffer_size, int *filesize)
//{
//	// consider alternative impl to speed up loading of large file
//	std::ifstream infile(filename);
//	if (infile.good()) {
//		std::stringstream sstream;
//		sstream << infile.rdbuf();
//		std::string file = sstream.str();
//
//		memcpy(buffer, file.data(), file.size());
//		*filesize = file.size();
//	}
//	else {
//		printf("[*] Fail to open file %s\n", filename);
//		*filesize = 0;
//	}
//
//	infile.close();
//}
//
//int main(int argc, char *argv[])
//{
//	Network*  net = new Network(SERV_IP);
//
//	char* str = "Hello server.";
//
//	Request *req = new Request(str, strlen(str) + 1);
//
//	net->send_request(req);
//
//	Response* res = net->recv_response();
//
//	printf("Get data: %s. Get size %d\n", res->get_data(), res->get_size());
//
//	char* buffer = new char[1024 * 1024* 10];
//	int fileSize = 0;
//	char* filename = "lightbox.pdf";
//	load_file(filename, buffer, sizeof(buffer), &fileSize);
//
//	printf("File: %s. File size %d\n", filename, fileSize);
//	req= new Request(buffer, fileSize);
//	net->send_request(req);
//
//	sleep(10);
//
//	return 0;
//}

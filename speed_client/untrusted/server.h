#ifndef SERVER_H
#define SERVER_H

void init_server();

void run_server(int id, const char* path, int count, int dedup);

void clear_server();

#endif
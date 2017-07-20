#ifndef CONFIG_H
#define CONFIG_H

#undef TEST_PUT_AND_GET

#include <stdint.h>

// all in bytes
#define REQ_TYPE_SIZE 1 // request type
#define RESP_TYPE_SIZE 1 // response type

#define HASH_SIZE 32 // sha256
#define TAG_SIZE HASH_SIZE
#define ENC_KEY_SIZE 16 // aes-gcm-128
#define MAC_SIZE 16 // aes-gcm-128
#define RAND_SIZE 32

// file buffer for I/O wrapper
#define FILE_BUFFER_SIZE 5*1024*1024 // 5M

// network
#define SERV_IP "192.168.1.5"
#define SERV_PORT 8888

#define TX_BUFFER_SIZE 1024*1024*10 // 10M
#define RX_BUFFER_SIZE 1024*1024*10 // 10M

// function types
#define FUNC_WC 1 // word count
#define FUNC_PM 2 // pattern matching
#define FUNC_CP 2 // compression
#endif
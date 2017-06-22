#ifndef CONFIG_H
#define CONFIG_H

// all in bytes
#define TAG_SIZE 32 // sha256
#define RAND_SIZE 16 // TBC
#define MAC_SIZE 32 // sha256

// file buffer for I/O wrapper
#define FILE_BUFFER_SIZE 1024*1024 // 1M

#endif
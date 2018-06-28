#pragma once
#include <stdlib.h>
#include <string.h>
#include "mapreduce.h"

#define DIC_SIZE 5000


int bow_run(int file_count, char **file_paths);
void bow_map(char *file_name);
void bow_reduce(char *key, Getter get_next, int partition_number);
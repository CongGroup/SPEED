#pragma once

#include <stdlib.h>
#include <string.h>
#include "mapreduce.h"

int wordcount_run(int argc, char *argv[]);
void wordcount_map(char *file_name);
void wordcount_reduce(char *key, Getter get_next, int partition_number);
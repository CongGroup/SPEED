#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mapreduce.h"
#include <assert.h>
#include <sys/types.h>
#include "sgx_thread.h"
#include "Enclave_t.h"
#include "sysutils.h"

// Max mapper count 809
#define MAPS 13

// Max reducer count 64
#define REDUCES 4



void *worker_mapper(void *);
void *worker_reducer(void *);
char *get_next(char *, int);
unsigned long MR_DefaultHashPartition(char *, int);
int comparator(const void *, const void *);

typedef struct __value_t_ value_t;
typedef struct __keys_t_ keys_t;
typedef struct __entry_t_ entry_t;
typedef struct __hashtable_t_ ht_t;

struct __value_t_ {
	char *value;
	struct __value_t_ *next;
};

struct __keys_t_ {
	char *key;
	value_t *head;
	struct __keys_t_ *next;
};

struct __entry_t_ {
	keys_t *head;
	sgx_thread_mutex_t lock;
};

struct __hashtable_t_ {
	entry_t map[MAPS];
	keys_t *sorted;
	int num_keys;
	int visit;
	sgx_thread_mutex_t lock;
};

int NUM_REDUCER;
// is same to mapper`s count
int NUM_FILES;
char **file_names;

Mapper mapper;
Reducer reducer;
Partitioner partitioner;

// counter next file to send to mapper in muti-thread
sgx_thread_mutex_t job_lock;
int map_done_count;

// same as readucer count
ht_t ht[REDUCES];

void initialize(int file_count, char ** file_pahts, Mapper mapp, int num_reducers, Partitioner part, Reducer red) 
{
	job_lock = SGX_THREAD_MUTEX_INITIALIZER;

	// Initialize globals
	NUM_FILES = file_count;
	file_names = file_pahts;
	mapper = mapp;
	map_done_count = 0;
	NUM_REDUCER = num_reducers;
	partitioner = part;
	reducer = red;

	// Initialize hash tables
	for (int i = 0; i < NUM_REDUCER; i++) 
	{
		ht[i].lock = SGX_THREAD_MUTEX_INITIALIZER;
		ht[i].num_keys = 0;
		ht[i].visit = 0;
		ht[i].sorted = NULL;

		for (int j = 0; j < MAPS; j++) 
		{
			ht[i].map[j].head = NULL;
			ht[i].map[j].lock = SGX_THREAD_MUTEX_INITIALIZER;
		}
	}

	// why do it???
	// Sort longest file first linear cuz why not
	//struct stat l, c;
	//// Ugly linear search cuz why not?
	//for (int i = 1; i < argc; i++) 
	//{
	//	int l_i = i;
	//	stat(argv[i], &l);
	//	for (int j = i + 1; j < argc; j++) 
	//	{
	//		stat(argv[j], &c);
	//		if (c.st_size > l.st_size) {
	//			stat(argv[j], &l);
	//			l_i = j;
	//		}
	//	}
	//	char *dup_i = argv[i];
	//	char *dup_l = argv[l_i];
	//	argv[i] = dup_l;
	//	argv[l_i] = dup_i;
	//}
}

void *worker_mapper(void *args) {

	while (1) {
		char *file;
		sgx_thread_mutex_lock(&job_lock);
		if (map_done_count >= NUM_FILES) {
			sgx_thread_mutex_unlock(&job_lock);
			return NULL;
		}
		file = file_names[map_done_count++];
		sgx_thread_mutex_unlock(&job_lock);
		(*mapper)(file);
	}
}

void *worker_reducer(void *args) {
	// args is reducer.ID
	int partition_num = *(int*)args;
	free(args);
	args = NULL;
	if (ht[partition_num].num_keys == 0)
		return NULL;
	ht[partition_num].sorted = (keys_t*)malloc(sizeof(keys_t) * ht[partition_num].num_keys);

	int counter = 0;
	for (int i = 0; i < MAPS; i++) {
		keys_t *curr = ht[partition_num].map[i].head;
		if (curr == NULL)
			continue;
		while (curr != NULL) {
			ht[partition_num].sorted[counter] = *curr;
			counter++;
			curr = curr->next;
		}
	}

	qsort(ht[partition_num].sorted, ht[partition_num].num_keys, sizeof(keys_t), comparator);

	for (int j = 0; j < ht[partition_num].num_keys; j++) 
	{
		char *key = ht[partition_num].sorted[j].key;

		(*reducer)(key, get_next, partition_num);
	}

	//free all memory
	for (int k = 0; k < MAPS; k++) {
		keys_t *curr = ht[partition_num].map[k].head;
		if (curr == NULL)
			continue;
		while (curr != NULL) 
		{
			free(curr->key);
			curr->key = NULL;

			value_t *curr_val = curr->head;
			while (curr_val != NULL) 
			{
				free(curr_val->value);
				curr_val->value = NULL;

				value_t *tmp = curr_val->next;
				free(curr_val);

				curr_val = tmp;
			}

			keys_t *tmp_key = curr->next;
			free(curr);

			curr = tmp_key;
		}
		curr = NULL;
	}
	free(ht[partition_num].sorted);
	ht[partition_num].sorted = NULL;

	return NULL;
}

int comparator(const void *a, const void *b) {
	char *a1 = ((keys_t*)a)->key;
	char *b1 = ((keys_t*)b)->key;

	return strcmp(a1, b1);
}

char *get_next(char *key, int partition_number) {
	keys_t *arr = ht[partition_number].sorted;
	char *value;

	while (1) 
	{
		int curr = ht[partition_number].visit;

		//search key in reduce`s sorted
		if (strcmp(arr[curr].key, key) == 0) 
		{
			if (arr[curr].head == NULL)
				return NULL;

			// return sorted[visit].head`s value and update it to it`s next
			value_t *tmp = arr[curr].head->next;
			value = arr[curr].head->value;
			arr[curr].head = tmp;
			return value;
		}
		else 
		{
			ht[partition_number].visit++;
			continue;
		}
		return NULL;
	}
}

unsigned long MR_DefaultHashPartition(char *key, int num_partitions) {
	unsigned long hash = 5381;
	int c;
	while ((c = *key++) != '\0')
		hash = hash * 33 + c;
	return hash % num_partitions;
}

void MR_Run(int argc, char *argv[],
	Mapper map, int num_mappers,
	Reducer reduce, int num_reducers,
	Partitioner partition) 
{
	// argc argv is file names;
	initialize(argc, argv, map, num_reducers, partition, reduce);

	ocall_begin_map(num_mappers);
	ocall_begin_reduce(num_reducers);

	
	//// create mapper threads
	//pthread_t threadsM[num_mappers];
	//for (int i = 0; i < num_mappers; i++) 
	//{
	//	pthread_create(&threadsM[i], NULL, worker_mapper, NULL);
	//}

	//// wait for threads to finish and join
	//for (int j = 0; j < num_mappers; j++) 
	//{
	//	pthread_join(threadsM[j], NULL);
	//}

	//// create reducer threads
	//pthread_t threadsR[num_reducers];
	//for (int k = 0; k < num_reducers; k++) {
	//	void *args = malloc(4);
	//	*(int*)args = k;
	//	pthread_create(&threadsR[k], NULL, worker_reducer, args);
	//}

	//for (int l = 0; l < num_reducers; l++) {
	//	pthread_join(threadsR[l], NULL);
	//}
}

// the value must be alloc in the memory.
void MR_Emit(char *key, char *value)
{
	unsigned long partition_num = (*partitioner)(key, NUM_REDUCER);
	unsigned long map_num = MR_DefaultHashPartition(key, MAPS);

	sgx_thread_mutex_lock(&ht[partition_num].map[map_num].lock);
	keys_t *tmp = ht[partition_num].map[map_num].head;
	//search key, if not exist go to keys end
	while (tmp != NULL)
	{
		if (strcmp(tmp->key, key) == 0)
			break;
		tmp = tmp->next;
	}

	value_t *new_val = (value_t*)malloc(sizeof(value_t));
	if (new_val == NULL) {
		sgx_thread_mutex_unlock(&ht[partition_num].map[map_num].lock);
		return;
	}

	//new_val->value = (char*)malloc(sizeof(char) * strlen(value)+1);
	//strncpy(new_val->value, value, strlen(value)+1);
	new_val->value = value;
	new_val->next = NULL;

	if (tmp == NULL) 
	{
		keys_t *new_key = (keys_t*)malloc(sizeof(keys_t));
		if (new_key == NULL) {
			sgx_thread_mutex_unlock(&ht[partition_num].map[map_num].lock);
			return;
		}
		new_key->head = new_val;
		new_key->next = ht[partition_num].map[map_num].head;
		ht[partition_num].map[map_num].head = new_key;

		new_key->key = (char*)malloc(sizeof(char) * strlen(key)+1);
		strncpy(new_key->key, key, strlen(key) + 1);
		sgx_thread_mutex_lock(&ht[partition_num].lock);
		ht[partition_num].num_keys++;
		sgx_thread_mutex_unlock(&ht[partition_num].lock);
	}
	else 
	{
		new_val->next = tmp->head;
		tmp->head = new_val;
	}

	sgx_thread_mutex_unlock(&ht[partition_num].map[map_num].lock);
}

void ecall_map_thread(int no)
{
	worker_mapper(0);
}

void ecall_reduce_thread(int no)
{
	void *args = malloc(4);
	*(int*)args = no;
	worker_reducer(args);
}

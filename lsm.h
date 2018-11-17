#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* To Simplify Implementation of LSM data structure, C0 will be simply array,
and C1 will be simple sorted array written to File*/

#define BUFFER_SIZE 5 //tuneable size

struct mem_node{
	char name[1024];
	char def[1024];
	int seq;
};


struct lsm{
	int num_node;
	struct mem_node buf[BUFFER_SIZE];
}c0;


pthread_mutex_t lsm_lock;
int node_comp(const void* a, const void* b);
void write_to_disk();
char* search_c0(char *key);
char* search_disk(char *key);
int lsm_insert(char* key, char* val);
char* lsm_get(char* key);
void lsm_delete(char* key);
void lsm_init();


//int node_comp(const void* a, const void*b);
//lsm* lsm_init();
//int lsm_insert(char* key, char* val);



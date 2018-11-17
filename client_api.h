#include <stdio.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#define PORT 8080

struct data_pair{
	char key[1024];
	char value[1024];
};
struct sockaddr_in *serv_addr;
pthread_mutex_t get_time_lock;
pthread_mutex_t put_time_lock;
void config_socket();
//Establish Connection, Send insert request, -1 if error
void *insert(void* dp);
//Establish Connection, Send delete request, -1 if error
void *del(void* key);
//Establish Connection, send get request, NULL if error
void *get(void *key);
//Establish Connection, send put request, NULL if error
void *put(void *dp);

void close_fd();

FILE *get_fd, *put_fd;

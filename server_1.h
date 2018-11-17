#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "lsm.h"



#define PORT 8080

#define CACHE_SIZE 40

#define THREAD_POOL_SIZE 1

struct continuation {
  int request_type; //0 for get 1 for put
  char buffer[1024];
  int fd;
  char result[1024];
  time_t start_time, finish_time;
}*temp;

struct pending_queue {
  struct continuation* cont;
  struct pending_queue *next;
}*pending_head, *pending_tail, *pending_node;

struct node {
  char *name;
  char *defn;
  struct node *next, *prev;
}*head,*tail,*curr,*temp_node;

struct sockaddr_in address;
pthread_mutex_t cache_lock;
int addrlen, opt = 1;
int *val,*incoming;
int *temp_val;
int initial_server_fd;
int cache_hit = 0;
int global_cache_count = 0;
pid_t my_pid;
union sigval *v;
char error[1024] = {"error"};
FILE *file;

/*function list*/
int max (int a, int b); // Helper function

char *strdups(char *s); // Helper function

struct node* get (char *s);

void put (char *name, char *defn);

void remove_node(char *name);

void *io_thread_func(); // To be completed

void *incoming_connection_handler(void *socket_desc); // To be completed

int server_func();

void event_loop();

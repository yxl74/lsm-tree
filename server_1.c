#include "server_1.h"

char *strdups(char *s) {/* make a duplicate of s */
    char *p;
    p = (char *) malloc(1024*sizeof(char)); /* +1 for ’\0’ */
    memset(p, 0, 1024);
    if (p != NULL)
       strncpy(p, s, 1024);
    return p;
}

void remove_node(char *name){
  struct node *garb;
  garb = get(name);
  if (garb != NULL){
    printf("remove CACHE LOCKING\n");
    pthread_mutex_lock(&cache_lock);
    if(garb->prev == NULL){
      head = garb->next;
      garb->next->prev = NULL;
    }
    else{
      garb->next->prev = garb->prev;
    }
    if(garb->next == NULL){
      tail = garb->prev;
      garb->prev->next = NULL;
    }
    else{
      garb->prev->next = garb->next;
    }
    free(garb->name);
    free(garb->defn);
    free(garb);
    garb = NULL;
    pthread_mutex_unlock(&cache_lock);
  }
}

struct node* get (char *s) {
  //printf("GET mutex Locking\n");
  pthread_mutex_lock(&cache_lock); //LOCKING CACHE, Updating entry
  curr = head;
  while (curr != NULL) {
    if (strcmp(curr->name, s) == 0) {
      //found in cache, move node to head of list
      // and return value of key
      if(curr != head) {
        temp_node = curr->prev;
        if (curr == tail) {
          temp_node->next = NULL;
          tail = temp_node;
        } else {
          temp_node->next = curr->next;
          curr->next->prev = temp_node;
        }

        head->prev = curr;
        curr->next = head;
        curr->prev = NULL;
        head = curr;
      }
      //printf("GET mutex UNlocking\n");
      cache_hit++;
      printf("Cache_hit: %d\n", cache_hit);
      pthread_mutex_unlock(&cache_lock);
      return head;
    }
    curr = curr->next;
  }
  //printf("GET mutex UNlocking\n");
  pthread_mutex_unlock(&cache_lock); //UNLOCKING Cache, Update Finish
  // Key not found in cache
  return NULL;
}

void put (char *name, char *defn) {
  struct node *cache_entry, *garbage;
  if ((cache_entry = get(name)) == NULL) {
    // value not in cache
    //printf("PUT CACHE LOCKING 1\n");
    pthread_mutex_lock(&cache_lock); //LOCKING CACHE, Updating entry
    temp_node = (struct node*) malloc (sizeof(struct node));
    temp_node->name = strdups(name);
    temp_node->defn = strdups(defn);
    temp_node->next = temp_node->prev = NULL;
    if(head == NULL) {
      head = tail = temp_node;
    } else {
      head->prev = temp_node;
      temp_node->next = head;
      temp_node->prev = NULL;
      head = temp_node;
    }
    if (global_cache_count >= CACHE_SIZE) {
      // cache is full, evict the last node
      // insert a new node in the list

      garbage = tail;

      tail = tail->prev;
      tail->next = NULL;

      //Free the memory of evict nodes
      free(garbage->name);
      free(garbage->defn);
      free(garbage);
      garbage = NULL;

     
    } else {
      // Increase the count
      global_cache_count++;
    }

  } else {
    //printf("PUT CACHE LOCKING 1\n");
    pthread_mutex_lock(&cache_lock); //LOCKING CACHE, Updating entry
    // update cache entry
    cache_entry->defn = strdups(defn);
  }
  //printf("PUT CACHE UNLOCKING\n");
  pthread_mutex_unlock(&cache_lock); //UNLOCKING Cache, Update Finish
}

//create a 
int server_func() {
  int server_fd;
  addrlen = sizeof(address);

  // Creating BLOCKING socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("socket failed");
      exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                &opt, sizeof(opt))) {
      perror("setsockopt");
      exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&address,
                               sizeof(address))<0) {
      perror("bind failed");
      exit(EXIT_FAILURE);
  }

  return server_fd;
}

void *incoming_connection_handler(void *socket_desc){
	int valread,request_type = -1;
	int sfd = *(int*)socket_desc;
	char *temp_string, *tokens, *garb;
	struct node *t_node;
  //memset(temp->result, 0, 1024); //reset temp node 
	printf("FD: %d\n", sfd);
  temp_string = (char *) malloc(1024 * sizeof(char));
  memset(temp_string, 0, 1024);
  //memset(temp -> buffer, 0, 1024);
  valread = read(sfd, temp_string, 1024);

  //strcpy(temp_string, temp->buffer); 
  garb = temp_string;
  tokens = strtok_r(temp_string, " ", &temp_string); //points to token(Operation)

  // 0 GET, 1 PUT, 2 insert, 3 delete
  
  if (strcmp(tokens, "GET") == 0){
  	request_type = 0;
  }
  else if(strcmp(tokens, "PUT") == 0){
  	request_type = 1;
  }
  else if(strcmp(tokens, "INSERT") == 0){
  	request_type = 2;
  }
  else if(strcmp(tokens, "DELETE") == 0){
  	request_type = 3;
  }
  printf("Request TYPE: %d\n", request_type);
  //Service Block

  //Service GET Request, Read only Request, "GET <key>"
  if(request_type == 0){
  	
  	printf("Handling GET\n");
  	tokens = strtok_r(temp_string, " ", &temp_string);
  	t_node = get(tokens); //checking cache
  	if(t_node != NULL){
  		//Result FOUND in cache
  		printf("\nResult found in cache\n\n");
  		send(sfd, t_node->defn, strlen(t_node->defn), 0);
  	}
  	else{
  		//Result NOT FOUND in cache, Perform IO in Blocking Fashion
  		char *def;
  		//printf("GET LOCKING FILE LOCK\n");
  		//printf("DB GET: %s\n", tokens);
  		def = lsm_get(tokens);
  		//printf("GET UNLOCKING FILE LOCK\n");
  		
  		if (def && strcmp(def, "DELETE") != 0){
  			send(sfd, def, sizeof(def), 0);
  			put(tokens, def);
  		}
  		else
  			send(sfd, "error", 5, 0); //ITEM DELETED 		
  	}
  }

  //SERVICE INSERT/PUT REQUEST
  else if(request_type == 1 || request_type == 2){
  	char* k1;
  	int res;
  	printf("Handling PUT\n");
  	tokens = strtok_r(temp_string, " ", &temp_string); //tokens now points to key
  	t_node = get(tokens);
  	k1 = tokens;
  	tokens = strtok_r(temp_string, " ", &temp_string);

  	//update cache if in cache
  	if(t_node != NULL){
  		strcpy(t_node->defn, tokens);
  	}
    else{
      put(k1, tokens);
    }
    lsm_insert(k1, tokens);
    send(sfd, "GOOD", 4, 0);
  }
  //SERVICE DELETE
  else if(request_type == 3){
  	printf("Handling DELETE\n");
  	tokens = strtok_r(temp_string, " ", &temp_string);

  	t_node = get(tokens);
  	if(t_node!= NULL){
  		remove_node(tokens);
  	}

  	lsm_delete(tokens);

  	send(sfd, "GOOD", 4, 0);
  }
  free(socket_desc);
  free(garb);
}

void event_loop(){
	initial_server_fd = server_func();
	struct sockaddr_in in;
	socklen_t sz = sizeof(in);

	if(listen(initial_server_fd, 5000) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}
	while(1){
		int *incoming;
		incoming = (int *) malloc(sizeof(int));
		printf("Waiting for a connection\n");
		//BLOCKING Socket IO Design
		*incoming = accept(initial_server_fd,(struct sockaddr*)&in, &sz);
		pthread_t handler_thread;
		printf("Handler Assigned\n");
		if (pthread_create(&handler_thread, NULL, incoming_connection_handler, (void*)incoming ) < 0){
			perror("Handler thread creation error\n");
			exit(EXIT_FAILURE);
		}
		
	}
}

int main(void)
{
	lsm_init();
	pthread_mutex_init(&cache_lock, NULL);
	event_loop();
}
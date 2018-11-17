#include "client_api.h"

void config_socket(){
	serv_addr = (struct sockaddr_in*) malloc (sizeof(struct sockaddr_in));
	memset(serv_addr, '0', sizeof(serv_addr));
	serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(PORT);
    if(inet_pton(AF_INET, "192.168.86.22", &serv_addr->sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
    }
		pthread_mutex_init(&get_time_lock, NULL);
		pthread_mutex_init(&put_time_lock, NULL);
		get_fd = fopen("./get_data.txt", "w");
		put_fd = fopen("./put_data.txt", "w");
}

void close_fd(){
	fclose(get_fd);
	fclose(put_fd);
}

void *insert(void* dp){
	struct data_pair *dpp = (struct data_pair*)dp;
	int *result = 0;
	char msg[1024] = {"INSERT "};
	char *key = dpp -> key;
	char *value = dpp -> value;
	strncat(msg, key, strlen(key));
	strncat(msg, " ", 1);
	strncat(msg, value, strlen(value));
	strncat(msg, " ", 1);
	//printf("Sending Request: %s\n", msg);

	int sock = 0, valread;
	char buffer[1024] = {0};

  	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  	{
      printf("\n Socket creation error \n");
      exit(0);
  	}

  	if (connect(sock, (struct sockaddr *)serv_addr, sizeof(struct sockaddr_in)) < 0)
  	{
      printf("\nConnection Failed \n");
      exit(0);
  	}

  	send(sock, msg, strlen(msg), 0);
  	printf("%lu REQUEST SENT: %s\n", pthread_self() ,msg);
  	valread = read(sock, buffer, 1024);
  	printf("%lu RESPONSE: %s\n",pthread_self() ,buffer);
    close(sock);
  	if(strcmp(buffer, "error") == 0){
  		return (void*) -1;
  	}
  	else
  		return (void*) 0;
}

void *del(void* key){
	char msg[1024] = {"DELETE "};
	strncat(msg, (char*)key, strlen((char*)key));
	strncat(msg, " ", 1);
	int sock = 0, valread;
	char buffer[1024] = {0};

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  	{
      printf("\n Socket creation error \n");
      exit(0);
  	}

  	if (connect(sock, (struct sockaddr *)serv_addr, sizeof(struct sockaddr_in)) < 0)
  	{
      printf("\nConnection Failed \n");
      exit(0);
  	}

  	send(sock, msg, strlen(msg), 0);
  	printf("%lu REQUEST SENT: %s\n", pthread_self() ,msg);
  	valread = read(sock, buffer, 1024);
  	printf("%lu RESPONSE: %s\n",pthread_self() ,buffer);
    close(sock);
  	if(strcmp(buffer, "error") == 0){
  		return (void*) -1;
  	}
  	else
  		return (void*) 0;
}

void *get(void *key){
	char msg[1024] = {"GET "};
	strncat(msg, (char*)key, strlen((char*)key));
	strncat(msg, " ", 1);
	int sock = 0, valread;
	char buffer[1024] = {0};

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  	{
      printf("\n Socket creation error \n");
      exit(0);
  	}

  	if (connect(sock, (struct sockaddr *)serv_addr, sizeof(struct sockaddr_in)) < 0)
  	{
      printf("\nConnection Failed \n");
      exit(0);
  	}
		struct timeval start, end;
		gettimeofday(&start, NULL);
  	send(sock, msg, strlen(msg), 0);
  	printf("%lu REQUEST SENT: %s\n", pthread_self() ,msg);
  	valread = read(sock, buffer, 1024);
  	printf("%lu RESPONSE: %s\n",pthread_self() ,buffer);
    close(sock);
		gettimeofday(&end, NULL);
		long seconds = (end.tv_sec - start.tv_sec);
		long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

		pthread_mutex_lock(&get_time_lock);
		fprintf(get_fd, "%ld,", micros);
		pthread_mutex_unlock(&get_time_lock);
  	if(strcmp(buffer, "error") == 0){
  		return NULL;
  	}
  	else
  		return (void*) 0;


}

void *put(void* dp){
	struct data_pair *dpp = (struct data_pair*)dp;
	char msg[1024] = {"PUT "};
	char *key = dpp -> key;
	char *value = dpp -> value;
	strncat(msg, key, strlen(key));
	strncat(msg, " ", 1);
	strncat(msg, value, strlen(value));
	strncat(msg, " ", 1);
	int sock = 0, valread;
	char buffer[1024] = {0};


	struct timeval start, end;
	gettimeofday(&start, NULL);
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  	{
      printf("\n Socket creation error \n");
      exit(0);
  	}

  	if (connect(sock, (struct sockaddr *)serv_addr, sizeof(struct sockaddr_in)) < 0)
  	{
      printf("\nConnection Failed \n");
      exit(0);
  	}

  	send(sock, msg, strlen(msg), 0);
  	printf("%lu REQUEST SENT: %s\n", pthread_self() ,msg);
  	valread = read(sock, buffer, 1024);
  	printf("%lu RESPONSE: %s\n",pthread_self() ,buffer);
    close(sock);
		gettimeofday(&end, NULL);
		long seconds = (end.tv_sec - start.tv_sec);
		long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
		pthread_mutex_lock(&put_time_lock);
		fprintf(put_fd, "%ld,", micros);
		pthread_mutex_unlock(&put_time_lock);
  	if(strcmp(buffer, "error") == 0){
  		return NULL;
  	}
  	else
  		return (void*) 0;



}

#include "client_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char* argv[]){
	config_socket();
	char a[20][1024];
	char b[180][1024];
	int n;
	pthread_t p_array[2000];
	n = atoi(argv[1]);

	// ARRAY INITIALIZATION
	int i = 0, j = 0, k = 0;
	FILE *fd = NULL;

	fd = fopen("./names.txt", "r");

	if(fd == NULL){
		printf("FAILLLL OPEN\n");
	}
	char* line, *tks;
	char c;
	line = (char *)malloc(1024*sizeof(char));
	char* garb = line;
	printf("HELLLO\n");
	while(fgets(line, 1024, fd) != NULL){
		if(i%10 == 0){
			tks = strtok_r(line, " ", &garb);
			strcpy(a[j], tks);
			printf("key inserted a: %s\n", a[j]);
			j++;
		}
		else{
			tks = strtok_r(line, " ", &garb);
			strcpy(b[k], tks);
			printf("key inserted b: %s\n", b[k]);
			k++;
		}
		i++;
	}
	free(line);
	fclose(fd);
	srand(time(0));
	//ARRAY INITIALIZATION


	struct timeval start, end;
	int num1, num2;
	gettimeofday(&start, NULL);
	char* key = (char *)malloc(1024*sizeof(char));
	struct data_pair *dp = (struct data_pair*) malloc(sizeof(struct data_pair));
	for(int l = 0; l < n; l++){
		num1 = rand()%10  + 1;
		printf("%d request: %d\n", l, num1);
		// Rare Key 10%
		if(num1 == 1){
			num1 = rand()%180; // from array b
			num2 = rand()%10+1; //50 50
			if(num2 != 1){
				strcpy(key, b[num1]);
				pthread_create(&p_array[l], NULL, get, (void*) key);
			}
			else{
				strcpy(dp->key, b[num1]);
				strcpy(dp->value, "HELLO");
				pthread_create(&p_array[l], NULL, put, (void*) dp);
			}
		}
		else{
			num1 = rand()%20;
			num2 = rand()%10+1;
			if(num2 != 1){
				strcpy(key, a[num1]);
				pthread_create(&p_array[l], NULL, get, (void*) key);
			}
			else{
				strcpy(dp->key, a[num1]);
				strcpy(dp->value, "HELLO");
				pthread_create(&p_array[l], NULL, put, (void*) dp);
			}
		}

	}//end for

	for(int i = 0; i < n; i++){
		pthread_join(p_array[i], NULL);
	}

	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	printf("Time elpased is %ld seconds and %ld micros\n", seconds, micros);
	//printf("Time per_request is: %f\n", (float)(seconds + micros/1000000)/n);
	close_fd();

}

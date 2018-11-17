#include "client_api.h"

int main(){
	config_socket();
	char key[1024] = {"Roger"};
	void *re1, *re2, *re3, *re4, *re5, *re6;
	struct data_pair *dp = (struct data_pair*) malloc(sizeof(struct data_pair));
	struct data_pair *dp1 = (struct data_pair*) malloc(sizeof(struct data_pair));
	strcpy(dp->key,"Eddy");
	strcpy(dp->value, "Fuck");
	pthread_t c1, c2, c3, c4, c5, c6;
	pthread_create(&c1, NULL, insert, (void*)dp);
	pthread_create(&c2, NULL, get, (void*) key);
	strcpy(dp1->key ,"Eddy");
	strcpy(dp1->value ,"Lee");
	pthread_create(&c3, NULL, put, (void*) dp1);
	pthread_create(&c4, NULL, get, (void*) key);
	//pthread_create(&c5, NULL, del, (void*) key);
	//pthread_create(&c6, NULL, get, (void*) key);
	pthread_join(c1, &re1);
	pthread_join(c2, &re2);
	pthread_join(c3, &re3);
	pthread_join(c4, &re4);
	//pthread_join(c5, &re5);
	//pthread_join(c6, &re6);
	free(dp);
	free(dp1);
	return 0;
}
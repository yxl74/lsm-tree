all: server_1 client_test s_test

server_1: server_1.h server_1.c lsm.h lsm.c
	gcc -g -std=gnu99 server_1.h server_1.c lsm.h lsm.c -o server_1 -pthread

client_test: test_client.c client_api.h client_api.c
	gcc -g -std=gnu99 test_client.c client_api.h client_api.c -o test_client -pthread

s_test: s_test.c client_api.h client_api.c 
	gcc -g -std=gnu99 s_test.c client_api.h client_api.c -o s_test -pthread

clean:
	rm test_client server_1 s_test



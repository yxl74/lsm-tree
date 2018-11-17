#include "lsm.h"//

int node_comp(const void* a, const void* b){
	const struct mem_node *na = (const struct mem_node*)a;
    const struct mem_node *nb = (const struct mem_node*)b;
    int res = strcmp(na->name, nb->name);
    if (res == 0)
    	return na->seq > nb->seq;
    else
    	return res;
}

static void print_c0(){
	printf("Printing c0\n");
	for(int i = 0; i < c0.num_node; i++){
		printf("print_c0:%s:%s\n", c0.buf[i].name,c0.buf[i].def);
	}
}



void write_to_disk(){
	printf("Writing to Disk\n");
	char *key, *value, *t_str;
	qsort(c0.buf, BUFFER_SIZE, sizeof(struct mem_node), node_comp);
	//print_c0();
	FILE *fp1 = fopen("cur_fp.txt", "r");
	char temp_str[1024];
	fgets(temp_str, 1024, fp1);
	int cur_fp = atoi(temp_str);
	fclose(fp1);
	FILE *fp;

	if (cur_fp == 0){
		//current consistent disk is disk.txt
		fp = fopen("disk.txt", "r");
		fp1 = fopen("disk1.txt", "w");
	}
	else{
		//current consistent disk is disk1.txt
		fp = fopen("disk1.txt", "r");
		fp1 = fopen("disk.txt", "w");
	}

	//Read Consistent Disk into Memory
	fgets(temp_str, 1024, fp);//frst line is the size of file
	int db_size = atoi(temp_str);
	struct mem_node t_buff[db_size];
	int counter = 0;
	while(fgets(temp_str, 1024, fp)){
		if(counter == db_size)
			break;
		key = strtok_r(temp_str, " ", &t_str);
		value = strtok_r(t_str, " ", &t_str);
		//printf("AT 46: %s:%s\n", key,value);
		//printf("AT 46: %s\n", temp_str);
		strcpy(t_buff[counter].name, key);
		strcpy(t_buff[counter].def, value);
		counter++;
	}
	//Finish Reading Consistent Disk into memory

	//Merge c0 and c1
	struct mem_node m_buff[BUFFER_SIZE + db_size];
	int real_size = 0, l = 0, r = 0;
	while(l < BUFFER_SIZE && r < db_size){
		int res = strcmp(c0.buf[l].name, t_buff[r].name);
		if( res < 0 || res == 0){
			//c0 go, need to check if any duplicate
			if(res == 0){
				//Could be DELETION
				if(strcmp(c0.buf[l].def, "DELETE") == 0){
					//DELETE OPERATION
					printf("DELETE OPERATION\n");
					r++;
					l++;
					real_size--;
					continue;
				}
			}
			while(strcmp(c0.buf[l].name, c0.buf[l+1].name) == 0){
				//printf("%s:%s\n", c0.buf[l].def,c0.buf[l+1].def);
				l++; //skipping older writes
			}
			if(strcmp(c0.buf[l].def, "DELETE") == 0){
				//DELETE but no key present
				l++;
				real_size--;
				continue;
			}
			strcpy(m_buff[real_size].name, c0.buf[l].name);
			strcpy(m_buff[real_size].def, c0.buf[l].def);
			l++;
			real_size++;
			
			if(res == 0)
				r++; //increment r as well, no need to write old key-value pair
		}
		else{
			strcpy(m_buff[real_size].name, t_buff[r].name);
			strcpy(m_buff[real_size].def, t_buff[r].def);
			r++;
			real_size++;
		}
	}
	//merge remaining c0 key-value pair
	while(l<BUFFER_SIZE){
		while(strcmp(c0.buf[l].name, c0.buf[l+1].name) == 0){
			l++; //skipping older writes
		}
		strcpy(m_buff[real_size].name, c0.buf[l].name);
		strcpy(m_buff[real_size].def, c0.buf[l].def);
		real_size++;
		l++;
	}
	//merge remaining key-valye pairs in file
	while(r<db_size){
		strcpy(m_buff[real_size].name, t_buff[r].name);
		strcpy(m_buff[real_size].def, t_buff[r].def);
		real_size++;
		r++;
	} 
	//write to disk
	fprintf(fp1, "%d\n", real_size);
	for(int j = 0; j < real_size; j++){
		//printf("AT 98:%s:%s\n", m_buff[j].name,m_buff[j].def);
		fprintf(fp1, "%s %s \n", m_buff[j].name,m_buff[j].def);
	}
	fclose(fp);
	fclose(fp1);
	//modify which file is consistent file "Assume atuomic?"

	fp = fopen("cur_fp.txt", "w");
	if(cur_fp == 0){
		fprintf(fp, "%s\n", "1");
	}
	else{
		fprintf(fp, "%s\n", "0");
	}
	c0.num_node = 0;
	fclose(fp);
}

char* search_c0(char *key){
	printf("C0 OPERATION\n");
	char *res = NULL;
	for(int i = 0; i < c0.num_node ; i++){
		if(strcmp(c0.buf[i].name, key) == 0){
			res = c0.buf[i].def;
		}
	}
	if(res != NULL)
		return res;
	for(int i = BUFFER_SIZE-1; i >= c0.num_node ; i--){
		if(strcmp(c0.buf[i].name, key) == 0){
			res = c0.buf[i].def;
		}
	}
	return res;
}

char* search_disk(char *key){
	printf("DISK OPERATION\n");
	char *key1, *value, *t_str;
	FILE *fp = fopen("cur_fp.txt", "r");
	char temp_str[1024] = {0};
	fgets(temp_str, 1024, fp);
	fclose(fp);
	int cur_fp = atoi(temp_str);

	if(cur_fp == 0){
		fp = fopen("disk.txt", "r");
	}
	else
		fp = fopen("disk1.txt", "r");
	fgets(temp_str, 1024, fp);
	int disk_size = atoi(temp_str);
	while(fgets(temp_str, 1024, fp)){
		key1 = strtok_r(temp_str," ", &t_str);
		value = strtok_r(t_str, " ", &t_str);
		if(strcmp(key,key1) == 0){
			return value;
		}
	}
	return NULL; 
}

int lsm_insert(char* key, char* val){
	pthread_mutex_lock(&lsm_lock);
	//printf("Inserting\n");
	if (c0.num_node == BUFFER_SIZE){
		//c0 full dump everything to disk, start to re-use slots from 0
		write_to_disk();
	}
	//printf("176: %d\n", c0.num_node);
	strcpy(c0.buf[c0.num_node].name, key);
	strcpy(c0.buf[c0.num_node].def, val);
	//printf("179: %d\n", c0.num_node);
	c0.buf[c0.num_node].seq = c0.num_node;
	//printf("179: %d\n", c0.num_node);
	c0.num_node++;
	pthread_mutex_unlock(&lsm_lock);
	return 0;
}

char* lsm_get(char* key){
	//Might returned DELETE
	pthread_mutex_lock(&lsm_lock);
	char *res = NULL;
	res = search_c0(key);
	if(res != NULL){
		pthread_mutex_unlock(&lsm_lock);
		return res;
	}
	res = search_disk(key);
	pthread_mutex_unlock(&lsm_lock);
	return res;
}


void lsm_delete(char* key){
	pthread_mutex_lock(&lsm_lock);
	if (c0.num_node == BUFFER_SIZE){
		//c0 full dump everything to disk, start to re-use slots from 0
		write_to_disk();
	}
	strcpy(c0.buf[c0.num_node].name, key);
	strcpy(c0.buf[c0.num_node].def, "DELETE"); //modify actualy disk later, maintain in c0, O(1)
	c0.buf[c0.num_node].seq = c0.num_node;
	c0.num_node++;
	//always sucess, 
	pthread_mutex_unlock(&lsm_lock);
}

void lsm_init(){
	pthread_mutex_init(&lsm_lock, NULL);
	c0.num_node = 0;
	FILE *fp1 = fopen("cur_fp.txt", "w");
	fprintf(fp1, "%s\n", "0"); //reference 0 as current consistent disk
	fclose(fp1);
	fp1 = fopen("disk.txt", "w");
	fprintf(fp1, "%s\n", "0");
	fclose(fp1);
	fp1 = fopen("disk1.txt", "w");
	fprintf(fp1, "%s\n", "0");
	fclose(fp1);

	FILE *fp = fopen("names.txt", "r");
	char *key, *value, *tmp_str;
	char line[1024] = {0};
	for (int i = 0; i < 200; i++){
		fgets(line, 1024,fp);
		key = strtok_r(line, " ", &tmp_str);
		value = strtok_r(tmp_str," ", &tmp_str);
		printf("%s:%s\n", key, value);
		lsm_insert(key,value);
	}
	fclose(fp);


}

#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include <time.h>
#define BUFFER_SIZE 2000000000L
#define THREADS 20
#define LEN 9

char* buffer[THREADS];
int dictionary_size;
char* dict[100000];
static long get_nanos(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
	return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}
int binsearch(char* dictionaryWords[],int listSize,char* keyword){
	int mid, low = 0, high = listSize - 1;
	while (high >= low) {
		mid = (high + low) / 2;
		if (strcmp(dictionaryWords[mid],keyword) < 0)
			low = mid + 1;
		else if (strcmp(dictionaryWords[mid],keyword)>0)
			high = mid - 1;
		else
			return mid;
	}
	return -1; //not found
}

//global assign, external variables
//typedef Struct{
//int id;
//int * length_list;
//int length;
//}
//thread_info;
//
//threadinfo all[THREADS]
//(void *)(all+thread_number ) --> thread_number

void* word_puzzle_solver(void* id){
	char c = 0;
	int i, n = BUFFER_SIZE/THREADS-LEN;
	char* buf = buffer[(int)id];
	fprintf(stderr, "Note: Thread #%d: %s\n", (int) id, "started!");
	for(i = 0; i < n;i++){
		if(c)
			buf[i + LEN - 1] = c;
		c = buf[i + LEN];
		buf[i + LEN] = '\0';
		//time = get_nanos();
		//lock before 45
		//unlock after 46
		if(binsearch(dict, dictionary_size, buf + i) + 1)//if search is successful!
			printf("Thread #%d: %s\n",(int)id, buf + i);
			//time = get_nanos() - time;
			//
	}
}
void main(int argc, char*** argv){
	long startTime = get_nanos();
	int thread_number, j, size = BUFFER_SIZE/THREADS;
	char temp[100];
	pthread_t threadID[THREADS];
	char line[1000];
	char * pointer;
	FILE* f = fopen("dict.txt", "r");
	dictionary_size = 0;
	while(fgets(line, 1000, f)){
		sscanf(line, "%s\n", temp);
		if(strlen(temp) == 0)
			continue;
		dict[dictionary_size] = (char*) malloc(sizeof(temp)+1);
		strcpy(dict[dictionary_size++], temp);
	}
	fclose(f);
	for(thread_number = 0; thread_number < THREADS;thread_number++){
		buffer[thread_number] = (char*) malloc(size + 1);
		if(!fgets(buffer[thread_number], size + 1, stdin)){
			fprintf(stderr, "Error: can't read the input stream!");
			break;
		}
		if(pthread_create(threadID + thread_number, NULL, word_puzzle_solver, (void *) thread_number)){
			fprintf(stderr, "Error: Too many threads are created!\n");
			break;
		}
	}
	for(j = 0; j < thread_number;j++){
		pthread_join(threadID[j], NULL);
		fprintf(stderr, "Note: Thread %d joined!\n", j);
	}
	fprintf(stderr, "Note: Total time: %ld nanoseconds using %d threads!\n", 
		get_nanos()-startTime, thread_number);
}
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include <time.h>
#define BUFFER_SIZE 2000000000 //2000000000L
#define THREADS 90
#define LEN 9

int flagL = 0;
int flagN = 0;
int flagV = 0;
int flagT = 0;

int numIndex = 0; 
int numThreads = 0;
int length[500];
int lenDefault[] = {8,9};

long stopWatch;
long searchTime;
pthread_mutex_t mutex;

char* buffer[THREADS]; //array that holds up to THREAD
int dictionary_size; 
char* dict[100000]; //array holds up to 100000


static long get_nanos(void) { //time
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
	return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

int binsearch(char* dictionaryWords[],int listSize,char* keyword){ //array size of list and a key
	int mid, low = 0, high = listSize - 1;
	while (high >= low) {
		mid = (high + low) / 2;
		if (strcmp(dictionaryWords[mid],keyword) < 0) // if word is found in upper part use that half
			low = mid + 1; //
		else if (strcmp(dictionaryWords[mid],keyword) > 0) // if word is found in lower half use that half
			high = mid - 1;
		else
			return mid; //found
	}
	return -1; //not found
}

void* word_puzzle_solver(void* id){
	int i, k;
	char* buf = buffer[(int)id];

	if(flagV){
		fprintf(stderr, "Note: Thread #%d: %s\n", (int) id, "started!");
	}

	for(k = 0; k < numIndex; k++) {
		char c = 0;
		int n = BUFFER_SIZE/numThreads-length[k];
		for(i = 0; i < n; i++){
			if(c) {
				buf[i + length[k] - 1] = c;
			}
			c = buf[i + length[k]];
			buf[i + length[k]] = '\0';

			stopWatch = get_nanos();
			if(binsearch(dict, dictionary_size, buf + i) + 1)//if search is successful!
				printf("Thread #%d: %s\n",(int)id, buf + i);

			stopWatch = get_nanos() - stopWatch;
			
			pthread_mutex_lock(&mutex);
			searchTime += stopWatch;
			pthread_mutex_unlock(&mutex);
		}
	}

}

void main(int argc, char** argv){ //char***
	long startTime = get_nanos(); //time starts

	pthread_mutex_init(&mutex, NULL);

	char * pointer;
	char * restLen;
	char * restThreads;
	char * wordLen;
	int l = 1;

	while(argv[l] != NULL){
		if (strcmp(argv[l], "-len") == 0 ){
			//length
			flagL = 1;
			// printf("\n\n-len\n"); 

			restLen = argv[l + 1];
			wordLen = strtok(restLen, ",");

			while(wordLen != NULL){
				length[numIndex++] = atoi(wordLen);
				wordLen = strtok(NULL, ",");
			}

		}

		else if (strcmp(argv[l], "-nthreads") == 0){
			//threads
			flagN = 1;
			// printf("\n-nthreads\n");
			restThreads = argv[l + 1];

			numThreads = atoi(restThreads);
		}

		else if (strcmp(argv[l], "-verbose") == 0){
			//verbose
			// printf("\n-verbose\n");
			flagV = 1;
			
		}
		else if (strcmp(argv[l], "-time") == 0){
			//time
			// printf("\n-time\n");
			flagT = 1;
		}
		l++;
	}

	int thread_number, j, size = BUFFER_SIZE/numThreads; 
	char temp[100];
	pthread_t threadID[numThreads];
	char line[1000];

	FILE* f = fopen("dict.txt", "r");
	dictionary_size = 0;

	while(fgets(line, 1000, f)){
		sscanf(line, "%s\n", temp);

		if(strlen(temp) == 0){
			continue;
		}

		dict[dictionary_size] = (char*) malloc(sizeof(temp)+1);
		strcpy(dict[dictionary_size++], temp);
	}
	fclose(f);

	for(thread_number = 0; thread_number < ((numThreads/ numIndex) * numIndex); thread_number++){
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

		if(flagV){
			fprintf(stderr, "Note: Thread %d joined!\n", j);
		}
		pthread_mutex_destroy(&mutex);
	}

	if(flagV){
		fprintf(stderr, "Note: Total time: %ld in seconds using %d threads!\n", 
			(get_nanos()-startTime) / 1000000000, numThreads);
	}

	if(flagT){
		printf("Total binary search time in seconds: %ld\n" , searchTime / 1000000000);
	}

}
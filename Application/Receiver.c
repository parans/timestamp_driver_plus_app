/*Functionality for receiving tokens, token receiving terminates after 100 tokens*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>
#include "Parser.h"

#define MAX_WORK_COUNT 100
#define SLEEP_TIME 10000

extern void parseData(char* buf);

int receiver_work_count=0;                                  //Shared variable b/w multiple routers, this variable keeps track of the no of times tokens are received
pthread_mutex_t receiver_wk_mx = PTHREAD_MUTEX_INITIALIZER; //Mutex lock associated with the shared variable

/*This method is used to increment #@receiver_work_count, this method is serialized*/
void incrReceiverWorkCount()
{
	pthread_mutex_lock(&receiver_wk_mx);
	receiver_work_count++;
	pthread_mutex_unlock(&receiver_wk_mx);
}

/* This method is used to get the value of #@receiver_work_count, this method is serialized to avoid reads while the value is being written*/
int getReceiverWorkCount()
{
	pthread_mutex_lock(&receiver_wk_mx);
	int count = receiver_work_count;
	pthread_mutex_unlock(&receiver_wk_mx);
	return count;
}

/* Reads data packets from the device*/
int receiveData(int fd, char* buf, int count){
	int status = 1;
	int res = read(fd, buf, count);
	if(res == -1)
	{
		printf("\n Queue is empty");
		status=0;
	}
	else if(res==0)
	{
		printf("\n Receiver -> Data Lost: Only %d bytes are read", count-res);
		status = 0;
	}
	return status;
}

/*This method is responsible for unpacking the data after reading from the device*/
void receiver_worker()
{
	int status, count = 0;
	char* buf = (char*)malloc(MAX_DATA_COUNT);

	/* Open device file*/
	int fd = open(DEVICE_FILE_2, O_RDONLY);
	if(fd<0)
	{
		printf("\nFailed to open device");
		return;
	}

	/*Read data packets intermittantly*/
	do
	{
		status = receiveData(fd, buf, MAX_DATA_COUNT);
		if(status)
            parseData(buf);
		incrReceiverWorkCount();
		usleep((rand()%41 + 10)*1000);
		count = getReceiverWorkCount();
		bzero(buf, MAX_DATA_COUNT);
	}while(count < MAX_WORK_COUNT);

	free(buf);
	close(fd);
}

// Entry point for the threads
void* dispatchReceiverWorker(void* ptr)
{
	receiver_worker();
}

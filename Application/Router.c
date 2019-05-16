/*Functionality to read data from 1 device, parse and print that data and route the data to another device*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include "Parser.h"

#define MAX_WORK_COUNT 100
#define SLEEP_TIME 10000

int router_work_count=0;                                   //Shared variable b/w multiple routers, this variable keeps track of the no of times tokens are routed
pthread_mutex_t router_wk_mx = PTHREAD_MUTEX_INITIALIZER;  //Mutex lock associated with the shared variable

extern void encapsulate_data(packet_t packet, int token_id, char* token_str);
extern int sendData(int fd, char* buf, int count);
extern int receiveData(int fd, char* buf, int count);
extern void parseData(char* buf);

/*This method is used to increment #@router_work_count, this method is serialized*/
void incrRouterWorkCount()
{
	pthread_mutex_lock(&router_wk_mx);
	router_work_count++;
	pthread_mutex_unlock(&router_wk_mx);
}

/* This method is used to get the value of #@router_work_count, this method is serialized to avoid reads while the value is being written*/
int getRouterWorkCount()
{
	pthread_mutex_lock(&router_wk_mx);
	int count = router_work_count;
	pthread_mutex_unlock(&router_wk_mx);
	return count;
}

/* Read data from 1 device, parse and print the data and write data to another device, after routing go to sleep and then repeat for 100 counts*/
void router_worker()
{
	int writeStatus = 1;
	int readStatus = 0;
	int count = 0;

	packet_t packet = (packet_t)malloc(sizeof(Data_Packet));
	packet->buffer = (char*)malloc(MAX_USR_DATA_COUNT);
	char* buf = (char*)malloc(MAX_DATA_COUNT);

	/* Open device file 1*/
	int fd1 = open(DEVICE_FILE_1, O_RDONLY);
	if(fd1<0)
	{
		printf("\nFailed to open device:%s", DEVICE_FILE_1);
		return;
	}

    /* Open device file 2*/
	int fd2 = open(DEVICE_FILE_2, O_WRONLY);
	if(fd2<0)
	{
		printf("\nFailed to open device%s", DEVICE_FILE_2);
		return;
	}
	/*Read data packets intermittantly*/
	do
	{
		if(writeStatus)
		{
                readStatus = receiveData(fd1, buf, MAX_DATA_COUNT);
                if(readStatus)
                {
                    parseData(buf);
                    encapsulate_data(packet, *(int*)buf, (char*)(buf+sizeof(int)));
                    writeStatus = sendData(fd2, packet->buffer, packet->data_count);
                }

		}
		else
		{
		    writeStatus = sendData(fd2, packet->buffer, packet->data_count);
		}
		incrRouterWorkCount();
		usleep((rand()%41 + 10)*1000);
		count = getRouterWorkCount();
		bzero(buf, MAX_DATA_COUNT);
	}while(count < MAX_WORK_COUNT);

	free(buf);
	free(packet);
	close(fd2);
	close(fd1);
}

//Thread entry point
void* dispatchRouterWorker(void* ptr)
{
	router_worker();
}



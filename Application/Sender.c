/*Functionality for sending tokens, token sending terminates after 100 tokens*/
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

extern void encapsulate_data(packet_t packet, int token_id, char* token_str);
extern int getGlobalTokenId();
extern void getTokenString(char* token_str, int seed);

int sender_work_count=0;                                    //Shared variable b/w multiple senders, this variable keeps track of the no of times tokens are sent
pthread_mutex_t sender_wk_mx = PTHREAD_MUTEX_INITIALIZER;   //Mutex lock associated with the shared variable

/*This method is used to increment #@sender_work_count, this method is serialized*/
void incrSenderWorkCount()
{
	pthread_mutex_lock(&sender_wk_mx);
	sender_work_count++;
	pthread_mutex_unlock(&sender_wk_mx);
}

/*This method is used to get the value of #@workcount, this method is serialized to avoid reads while the value is being written*/
int getSenderWorkCount()
{
	pthread_mutex_lock(&sender_wk_mx);
	int count = sender_work_count;
	pthread_mutex_unlock(&sender_wk_mx);
	return count;
}

/* Writes data packet to the device*/
int sendData(int fd, char* buf, int count)
{
	int status = 1;

	int res = write(fd, buf, count);
	if(res == -1)
	{
		printf("\n Queue is full");
		status = 0;
	}
	else if(res<count)
	{
		printf("\n Sender -> Data Lost: Only %d bytes are written", res);
		status = 0;
	}
	return status;
}

// This method is responsible to get token id and token str and pack the data before it is sent to the device
void sender_worker()
{
	int status, token_id, count = 0;
	char* token_str = (char*)malloc(80);

	packet_t packet = (packet_t)malloc(sizeof(Data_Packet));
	packet->buffer = (char*)malloc(MAX_USR_DATA_COUNT);

	/* Open device file*/
	int fd = open(DEVICE_FILE_1, O_WRONLY);
	if(fd<0)
	{
		printf("\nFailed to open device");
		return;
	}

	/*Write data packets intermittantly*/
	status =1;
	do
	{
		if(status)
		{
            token_id = getGlobalTokenId();
            getTokenString(token_str, count);
            encapsulate_data(packet, token_id, token_str);
		}
		status = sendData(fd, packet->buffer, packet->data_count);
        	incrSenderWorkCount();
		usleep((rand()%41 + 10)*1000);
		count = getSenderWorkCount();
	}while( count < MAX_WORK_COUNT);
	/*Stop after 100 packets*/
    free(packet);
    free(token_str);
	close(fd);
}

//Thread entry point
void* dispatchSenderWorker(void* ptr)
{
	sender_worker();
}


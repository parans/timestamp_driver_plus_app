#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NO_OF_SENDERS 2
#define NO_OF_RECEIVERS 2
#define NO_OF_ROUTERS 2

extern void* dispatchSenderWorker(void*);
extern void* dispatchReceiverWorker(void*);
extern void* dispatchRouterWorker(void*);

int main(int argc, char** argv)
{
	pthread_t sender[NO_OF_SENDERS];
	pthread_t receiver[NO_OF_RECEIVERS];
	pthread_t router[NO_OF_ROUTERS];

	int i, status=0;

	/*Create Sender threads*/
	for(i=0;i<NO_OF_SENDERS;i++)
	{
		status = pthread_create(&sender[i], NULL, &dispatchSenderWorker, NULL);
		if(status)
		{
			printf("\nSender%d creation failure", i);
			exit(0);
		}
	}

	/*Create Router threads*/
	for(i=0;i<NO_OF_ROUTERS;i++)
	{
		status = pthread_create(&router[i], NULL, &dispatchRouterWorker, NULL);
		if(status)
		{
			printf("\nRouter%d creation failure", i);
			exit(0);
		}
	}

	/*Create Receiver threads*/
	for(i=0;i<NO_OF_RECEIVERS;i++)
	{
		status = pthread_create(&receiver[i], NULL, &dispatchReceiverWorker, NULL);
		if(status)
		{
			printf("\nReceiver%d creation failure", i);
			exit(0);
		}
	}
	/*Wait for senders*/
	for(i=0;i<NO_OF_SENDERS;i++)
	{
		status = pthread_join(sender[i], NULL);
		if(status)
		{
			printf("\nSender%d join failure", i);
			exit(0);
		}
	}
    /*Wait for Routers*/
    for(i=0;i<NO_OF_ROUTERS;i++)
	{
		status = pthread_join(router[i], NULL);
		if(status)
		{
			printf("\nRouter%d join failure", i);
			exit(0);
		}
	}

	/*Wait for receivers*/
	for(i=0;i<NO_OF_RECEIVERS;i++)
	{
		status = pthread_join(receiver[i], NULL);
		if(status)
		{
			printf("\nReceiver%d join failure", i);
			exit(0);
		}
	}
	return 0;
}

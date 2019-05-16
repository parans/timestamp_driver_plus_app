#include <mqueue.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    mqd_t fd_mQ;
    int j;
    pid_t child_pid;
    char *outBuf;
    char inBuf[50];
    unsigned int priority = 1;

       struct mq_attr attr;

    int must_stop = 0;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 50;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    fd_mQ = mq_open("/MesgQ",O_RDWR|O_CREAT, 0777, &attr);



   // fd_mQ = mq_open("MesgQ",O_RDWR);
    if (fd_mQ == -1)
    {
        perror("File cannot be opened");
        return -1;
    }

    for(j=0;j<50;j++)
            inBuf[j] = 'B';
    inBuf[49] = 0;
	printf("       mQueue\n");
	printf("SENDING  %s \n",inBuf);
	outBuf = malloc(50);
    	for(j=0;j<500;j++)
    	{
    		mq_send(fd_mQ,inBuf,50,1);
    		int len =  mq_receive(fd_mQ,outBuf,50,&priority);
    		printf("RECEIVED %s",outBuf);
    	}

      /* Wait for child */
        exit(0);

}

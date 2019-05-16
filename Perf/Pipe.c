#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int pipefd[2],j;
    char* outBuf;
    char inBuf[50];


    for(j=0;j<50;j++)
            inBuf[j] = 'C';
    inBuf[49] = 0;

    if (pipe(pipefd) == -1) {
        perror("Could not get file descriptors");
        exit(EXIT_FAILURE);
    }
        printf("         Pipe\n");
        outBuf = malloc(50);
	printf("SENDING  %s \n",inBuf);
	for(j=0;j<500;j++)
    	{
        	write(pipefd[1], inBuf, 50);
        	read(pipefd[0],outBuf,50) ; //read character from the pipe to the buffer character
        	printf("RECEIVED %s",outBuf);
	}
        free(outBuf);
        close(pipefd[0]);                       //close Read file descriptor


        close(pipefd[1]);       /* Reader will see EOF */

        exit(0);

}

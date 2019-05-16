#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#define DEVICE1 "/dev/Tqueue1"

int main(int argc, char *argv[])
{
    int fd, i, j;
    char *outBuf;
    char inBuf[50];


    for(j=0;j<50;j++)
            inBuf[j] = 'D';
    inBuf[49] = '\0';

    fd = open(DEVICE1,O_RDWR);
    if (fd == -1)
    {
        perror("File cannot be opened");
        return -1;
    }
	printf("Device Driver\n");
	outBuf = malloc(56);
	printf("SENDING  %s \n",inBuf);
	for(i=0;i<500;i++)
	{
		int len  = write(fd, inBuf, 50);
		len = read(fd,outBuf,0); //read character from the pipe to the buffer character
		printf("%s",outBuf+sizeof(int));
	}
        close(fd);
        exit(0);

}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "usr_timestamp.h"

#define DEVICE_FILE_1 "/dev/Tqueue1"
#define DEVICE_FILE_2 "/dev/Tqueue2"
#define MAX_DATA_COUNT 96
#define MIN_DATA_COUNT 20
#define MAX_USR_DATA_COUNT 84

/*This data structure is used by the parser to encapsulate token_id+token_string*/
typedef struct {
	char* buffer;       /*This buffer stores token_id and tokenstring*/
	int data_count;     /*This stores the length of tokenid+token str without \0*/
}Data_Packet, *packet_t;

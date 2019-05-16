#include "Parser.h"

pthread_mutex_t out_stream_mx = PTHREAD_MUTEX_INITIALIZER;

/*print token data to the console, this method is serialized to avoid garbled messages on screen*/
void outData(int token_id, char* token_str, unsigned char* a_ts, unsigned char* d_ts)
{
	pthread_mutex_lock(&out_stream_mx);
	printf("\nToken id: %d\tArrival %x:%x:%x\tDeparture %x:%x:%x\t\tToken_str: %s", token_id, a_ts[0], a_ts[1], a_ts[2], d_ts[0], d_ts[1], d_ts[2], token_str);
	pthread_mutex_unlock(&out_stream_mx);
}

/*Packs token_id an token string into a char* buffer*/
void encapsulate_data(packet_t packet, int token_id, char* token_str)
{
	int index=0;
	int token_str_len = strlen(token_str);

	memcpy(packet->buffer, &token_id, sizeof(token_id));
	index+=sizeof(token_id);

	memcpy(packet->buffer+index, token_str, token_str_len);
	packet->data_count = index + token_str_len;
}

/*decapsulates the data received from the device and parses them into user readable format*/
void parseData(U_TS_Data_t ts_data, char* buf)
{
    int i=0;
    unsigned char a_ts[4], d_ts[4];
    int output_len = *(int*)buf;
    int token_id = *(int*)(buf+sizeof(output_len));
    int usr_str_len = output_len - (sizeof(output_len)+sizeof(token_id)+6);
    int index = (sizeof(output_len)+sizeof(token_id));


    char* usr_str = (char*)malloc(usr_str_len + 1);

    for(;i<usr_str_len; i++, index++)
    {
        usr_str[i] = *(buf+index);
    }
    usr_str[i] = '\0';
    a_ts[0] = *(buf+index);
    index++;
    a_ts[1] = *(buf+index);
    index++;
    a_ts[2] = *(buf+index);
    a_ts[3] = '\0';
    index++;

    d_ts[0] = *(buf+index);
    index++;
    d_ts[1] = *(buf+index);
    index++;
    d_ts[2] = *(buf+index);
    d_ts[3] = '\0';
    index++;

    outData(token_id, usr_str, a_ts, d_ts);

    bzero(buf, 96);
    memcpy(buf, &token_id, sizeof(token_id));
    memcpy(buf+sizeof(token_id), usr_str, usr_str_len);
    free(usr_str);
}


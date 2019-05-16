/* Includes functionalty to generate tokens, and a random token str; Token generation is serialized to ensure generation of unique token ids*/
#include <pthread.h>

#define TOKEN_STR_COUNT 5
#define MAX_STR_LEN 80
#define MIN_STR_LEN 10

int base_values[] = {64, 47, 96};
int bound_values[] = {26, 9, 26};

int token_id = 1;
pthread_mutex_t token_id_mx=PTHREAD_MUTEX_INITIALIZER;

/*This method generates unique token id, this methos is serailized to ensure unique token id generation*/
int getGlobalTokenId()
{
	int token=0;
	pthread_mutex_lock(&token_id_mx);
	token = token_id++;
	pthread_mutex_unlock(&token_id_mx);
	return token;
}

/*This method generated random alphanumeric strings of random sizes*/
void getTokenString(char* token_str, int seed)
{
        int i;
        srand(seed+1);
        int strlen = rand() % (MAX_STR_LEN-MIN_STR_LEN) + 10;
        for(i=0;i<strlen;i++)
            token_str[i] = (char)(rand()%bound_values[i%3] + base_values[i%3]+1);
        token_str[i] = '\0';
}

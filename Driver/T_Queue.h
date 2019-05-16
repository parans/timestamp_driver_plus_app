#define BUFFER_SIZE 256

typedef struct{
	int head;
	int tail;
	char *buf;
}T_Queue, *T_Q;

T_Q initialize_TQueue()
{
	T_Q q = kmalloc(sizeof(T_Queue), GFP_KERNEL);
	if(q==NULL)
	{
		printk("\n Unable to allocate memory to the queue");
		return NULL;
	}
	q->buf = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	if(q->buf == NULL)
	{
		printk("\n Unable to allocate memory to the queue buffer");
		kfree(q);
		return NULL;
	}
	q->head = 0;
	q->tail = 0;
	return q;
}

int isQueueFull(T_Q q, int len)
{
	int mem_available = 0;
	int queueFull = 0;
	if(q->head==0&&q->tail==0)
        	mem_available = BUFFER_SIZE;
	else if(q->head < q->tail)
		mem_available = (BUFFER_SIZE - q->tail) + q->head;
	else if(q->head > q->tail)
		mem_available = q->head - q->tail;
	if(mem_available < len)
		queueFull = 1;
	return queueFull;
}

int T_enqueue(char* kbuf, T_Q q)
{
	int len, memory_deficit = 0;
	len = *((int*)kbuf);
	if(isQueueFull(q, len))
		return -1;
	else if(q->head < q->tail)
	{
		memory_deficit = BUFFER_SIZE-q->tail;
		if(memory_deficit < len)
		{
			memcpy(q->buf+q->tail, kbuf, memory_deficit);
			q->tail = (q->tail+memory_deficit)%BUFFER_SIZE;
			memcpy(q->buf+q->tail, kbuf+memory_deficit,len-memory_deficit);
			q->tail+=len-memory_deficit;
			return len;
		}
		else if(memory_deficit == 0)
		{
			q->tail = (q->tail+memory_deficit)%BUFFER_SIZE;
			memcpy(q->buf+q->tail, kbuf+memory_deficit,len-memory_deficit);
			q->tail+=len-memory_deficit;
			return len;			
		}
	}
	memcpy(q->buf+q->tail, kbuf, len);
	q->tail+=len;
	return len;
}


int T_dequeue(T_Q q, char* data)
{
	int index=0;
	int len=0;	
	int data_read = 0;

	char* temp = kmalloc(sizeof(int), GFP_KERNEL);
	
	printk("\nDequeue on entry q->head:%d;q->tail:%d", q->head, q->tail);
	
	if(q->head == 0 && q->tail == 0)		//Queue empty condition
		return -1;	
	
	if(BUFFER_SIZE-q->head<sizeof(int))
	{
		memcpy(temp, q->buf+q->head, BUFFER_SIZE-q->head);
		index = BUFFER_SIZE-q->head;
		q->head = 0;
		memcpy(temp+index, q->buf+q->head, sizeof(int)-index);
		memcpy(data, temp, sizeof(int));
		q->head = sizeof(int)-index;
		len = *(int*)temp - sizeof(int);
			
		if(q->head+len == q->tail)
		{
			memcpy(data+sizeof(int), q->buf+q->head, len);
			q->head = q->tail = 0;				
		}
		else
		{
			memcpy(data+sizeof(int), q->buf+q->head, len);   //count has been already copied so copy the rest
			q->head+=len;

		}	
		data_read = *(int*)temp;					
	}
	else
	{
		len = *((int*)(q->buf+q->head));
		if(q->head+len==BUFFER_SIZE)
		{
			memcpy(data, q->buf+q->head, len);
			q->head = (q->head+len)%BUFFER_SIZE;
		}
		else if(q->head+len > BUFFER_SIZE)
		{				
			memcpy(data, q->buf+q->head, BUFFER_SIZE-q->head);
			index =  BUFFER_SIZE-q->head;
			q->head = 0;
			memcpy(data+index, q->buf+q->head, len-index);
			q->head+= len-index;
			if(q->head == q->tail)
			{
				q->head = q->tail = 0;					
			} 			
		}
		else
		{
			if(q->head+len == q->tail)
			{
				memcpy(data, q->buf+q->head, len);
				q->head = q->tail = 0;	
			}
			else
			{
				memcpy(data, q->buf+q->head, len);
				q->head+=len;
			}			
		}
		data_read = len;
						
	}
	printk("\n data:%d", *(int*)(data));
	printk("\nDequeue on exit q->head:%d;q->tail:%d; data_read:%d", q->head, q->tail, data_read);
	kfree(temp);
	return data_read;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "queue.h"

PQueue queue_init(int ele_size, int capacity, int incsize)
{
	if( 0 >= ele_size ) return NULL; //bad parameter!

	PQueue pqueue = NULL;

	pqueue = (PQueue) malloc(sizeof(Queue)); //here is the struct of Queue!!
	if( NULL == pqueue ) {
		return NULL;
	}

	pqueue->ele_size = ele_size;

	if( 0 < capacity ){
		capacity ++;  //add one 
		pqueue->capacity = capacity;
	} else {
		pqueue->capacity = INIT_CAPACITY; 
	}

	if( 0 < incsize ){
		pqueue->incsize = incsize;
	} else {
		pqueue->incsize = INIT_INCSIZE;
	}

	pqueue->ele_size = ele_size;

	pqueue->data = (void*) malloc((pqueue->ele_size) * (pqueue->capacity) );
	if( NULL == pqueue->data ){
		free(pqueue);
		return NULL;
	}


	//init data field!
	pqueue->front = pqueue->rear=0;

	return pqueue;

}

int queue_is_empty(PQueue pqueue)
{
	if( pqueue->front == pqueue->rear ) return 1;
	return 0;
}

int queue_is_full(PQueue pqueue)
{
	//if( (pqueue->rear + pqueue->capacity - pqueue->front + 1)%(pqueue->capacity) == 0 ) return 1;
	if( (pqueue->rear + 1) == pqueue->front ) return 1;
	else if( ((pqueue->rear +1) ==pqueue->capacity) && (pqueue->front == 0 )) return 1;
	return 0;
}


int queue_push(PQueue pqueue, void * data)
{
	if( NULL == pqueue ){
		return 0;
	}

	if( queue_is_full( pqueue ) ){
		//resize the capacity
		void* pdata = (void*) malloc((pqueue->ele_size) * (pqueue->capacity+pqueue->incsize));
		if( NULL == pdata ) return 0; //can not allocate memory!
		
		int k=0;
		void* index = pdata;

		for(k=pqueue->front; k<pqueue->rear && k<pqueue->capacity; k++){
			//pdata[index]=pqueue->data[k];
			memcpy( index, pqueue->data + k*(pqueue->ele_size), pqueue->ele_size);
			index += pqueue->ele_size;
		}
		if( pqueue->rear < pqueue->front ){
			for(k=0;k<pqueue->rear;k++) {
				//pdata[index] = pqueue->data[k];
				//index++;

				memcpy( index, pqueue->data + k*(pqueue->ele_size), pqueue->ele_size); 
				index += pqueue->ele_size;
			}
		}
		//just for test!
		printf("重新分配内存, 原来大小=%d, 增量=%d \n", pqueue->capacity, pqueue->incsize);

		//free old memory!
		free(pqueue->data);
		pqueue->data=pdata;

		pqueue->front=0;
		pqueue->rear= pqueue->capacity-1;
		//pqueue->rear=index; // or pqueue->capacity-1;

		pqueue->capacity+=pqueue->incsize;
	}

	memcpy( pqueue->data + pqueue->rear*pqueue->ele_size,  data, pqueue->ele_size); 
	//pqueue->data[pqueue->rear] = data;
	pqueue->rear = (pqueue->rear+1)%pqueue->capacity;
	
	return 1;	
}

void* queue_pop(PQueue pqueue)
{
	void* data = NULL;
	if( NULL == pqueue ) return NULL;

	if( queue_is_empty(pqueue) ) return NULL;
	
	//data = pqueue->data[pqueue->front];
	data = pqueue->data + pqueue->front*pqueue->ele_size;

	pqueue->front = (pqueue->front+1)%pqueue->capacity;
	return data;
}

void queue_destroy(PQueue pqueue)
{
	if( NULL == pqueue ) return;

	free(pqueue->data);
	free(pqueue);
	return;
}

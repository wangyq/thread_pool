
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

#ifndef QUEUE_H
#define QUEUE_H

//queue definition
//

#define INIT_CAPACITY 32
#define INIT_INCSIZE  16

typedef struct queue{
	void* data;         //存储的数据域
	size_t ele_size;       //每个存储的数据域的大小
	int front,rear;     //头尾指针
	int capacity, incsize;   //容量和每次增加的大小
}Queue, *PQueue;


/**
 * parameter:
 * 	capacity - init capacity, zero to using default INIT_CAPACITY
 * 	incsize  - incsize ,      zero to using default INIT_INCSIZE
*/
PQueue queue_init(int ele_size, int capacity, int incsize);

/**
 * push data into queue
 * return result:
 * 	0 - mean false, otherwise true
 *
 */

/**
 * destroy queue!
 *
 */
void queue_destroy(PQueue pqueue);

int queue_push(PQueue pqueue, void* data);

/**
 * pop data from queue
 * return result:
 * 	NULL - mean false, otherwise the return value is the data of the front location of queue
 *
 */
void* queue_pop(PQueue pqueue);


/**
 * return result:
 * 	1 - if queue is empty, 0 otherwise
 */
int queue_is_empty(PQueue pqueue);

/**
 * return result
 * 	1 - if queue is full, 0 otherwise
 *
 */
int queue_is_full(PQueue pqueue);




#endif

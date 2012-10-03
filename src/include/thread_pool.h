#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>
#include "queue.h"

#ifndef THREAD_POOL_H
#define THREAD_POOL_H


/**
 * Task function definition
 *
 */
typedef void* (*PTaskFun)(void* arg);


typedef struct tag_thread_pool{
	pthread_t *threadid;  //running thread's id
	int max_size;   //最大线程池容量
	int cur_size;   //当前的线程数目
	int busy_size;  //当前忙碌的线程数目
	int lazy_init;  //是否使用lazy_init 功能

	PQueue task_queue;    //Task's queue
	pthread_mutex_t task_lock; //lock to access Task's queue
	pthread_cond_t task_ready; // tell thread_pool that has already task


	/*是否销毁线程池*/
	int is_shutdown;
}ThreadPool, *PThreadPool;

/**
 *
 *
 *
 */
PThreadPool thread_pool_init(int max_size, int lazy_init);

/**
 *
 *
 */
void thread_pool_destroy(PThreadPool pool);

int thread_pool_add_task(PThreadPool pool,PTaskFun fun, void* arg);



#endif

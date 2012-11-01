#include <stdlib.h>
#include <stdio.h>

#include "thread_pool.h"

typedef struct tag_task {
	PTaskFun fun;
	void* arg;
} Task, *PTask;

static void* thread_pool_routine(void *arg) {
	if (NULL == arg)
		return NULL;
	PThreadPool pool = (PThreadPool) arg;

	printf("starting Pool=%p 's thread 0x%x\n", (void*) pool,(int) pthread_self());
	while (1) {
		pthread_mutex_lock(&(pool->task_lock));
		/*如果等待队列为0并且不销毁线程池，则处于阻塞状态; 注意 
		 pthread_cond_wait是一个原子操作，等待前会解锁，唤醒后会加锁*/
		while (queue_is_empty(pool->task_queue) && !pool->is_shutdown) {
			printf("Pool=%p 's  thread 0x%x is waiting\n", (void*) pool,(int) pthread_self());

			if( !pool->is_finished ){
				pool->is_finished = 1;
				pthread_cond_signal(&(pool->task_finished));//唤醒等待线程

			}
			//must lock before call this funcion.
			//check the shared variable "task_ready" protected by "task_lock",
			//and unlock "queue_lock" when go into sleep.
			//otherwise , lock it and return immediate when get signal!
			pthread_cond_wait(&(pool->task_ready), &(pool->task_lock));
		}

		/*线程池要销毁了*/
		if (pool->is_shutdown) {
			/*遇到break,continue,return等跳转语句，千万不要忘记先解锁*/
			pthread_mutex_unlock(&(pool->task_lock));
			printf("Pool=%p 's thread 0x%x will exit\n", (void*) pool,(int) pthread_self());
			pthread_exit(NULL);
		}

		printf("Pool=%p 's thread 0x%x is going to work\n", (void*) pool,(int) pthread_self());

		/*assert是调试的好帮手*/
		assert(!queue_is_empty(pool->task_queue));

		/*等待队列长度减去1，并取出链表中的头元素*/
		//这里必须拷贝出任务，方能执行
		Task task = *((PTask) queue_pop(pool->task_queue));
		pthread_mutex_unlock(&(pool->task_lock));

		//assert( NULL != ptask );
		/*调用回调函数，执行任务*/
		((task.fun))(task.arg);

	}
	/*这一句应该是不可达的*/
	pthread_exit(NULL);
}

/**
 * 调用线程阻塞, 直到所有任务结束, 此函数才返回.
 *
 */
void thread_pool_await_finished(PThreadPool pool)
{
	pthread_mutex_lock(&(pool->task_lock)); //获取锁
	pthread_cond_wait(&(pool->task_finished), &(pool->task_lock)); //等待task finished!
	pthread_mutex_unlock(&(pool->task_lock));//释放锁
}

void thread_pool_destroy(PThreadPool pool) {
	int i = 0;

	//pthread_mutex_lock(&(pool->task_lock)); //获取锁 no need to do this!
	pool->is_shutdown = 1;                  //结束线程
	pthread_cond_broadcast(&(pool->task_ready));
	//pthread_mutex_unlock(&(pool->task_lock));//释放锁

	//here maybe we should wait thread to terminated using thread_join() call.
	//or using sleep() call
	for ( i=0; i<pool->max_size; i++ ){
		pthread_join(pool->threadid[i], NULL);
	}

	if (NULL != pool) {
		queue_destroy(pool->task_queue);
		free(pool->threadid);
		free(pool);
	}
}

PThreadPool thread_pool_init(int max_size, int lazy_init) {
	PThreadPool pool = NULL;
	if (0 >= max_size)
		return NULL;

	pool = (PThreadPool) malloc(sizeof(ThreadPool));
	if (NULL == pool)
		return NULL;

	pool->threadid = (pthread_t *) malloc(max_size * sizeof(pthread_t));
	if (NULL == pool->threadid) {
		free(pool);
		return NULL;
	}
	pool->task_queue = queue_init(sizeof(Task), 0, 0);
	if (NULL == pool->task_queue) {
		free(pool->threadid);
		free(pool);
		return NULL;
	}

	pool->max_size = max_size;
	pool->is_shutdown = 0;
	pool->is_finished = 0;

	//init parameter
	pthread_mutex_init(&(pool->task_lock), NULL);
	pthread_cond_init(&(pool->task_ready), NULL);
	pthread_cond_init(&(pool->task_finished), NULL);

	int i = 0;
	for (i = 0; i < pool->max_size; i++) {
		pthread_create(&(pool->threadid[i]), NULL, thread_pool_routine,
				(void*) pool);
	}

	return pool;
}

int thread_pool_add_task(PThreadPool pool, PTaskFun fun, void* arg) {
	if ( (NULL == pool) || (NULL == fun) )
		return 0;
	Task task = { fun, arg };

	//
	pthread_mutex_lock(&(pool->task_lock));
	queue_push(pool->task_queue, (void*) &task);
	pool->is_finished = 0;                     //结束条件为0

	/*好了，等待队列中有任务了，唤醒一个等待线程；
	 注意如果所有线程都在忙碌，这句没有任何作用*/
	pthread_cond_signal(&(pool->task_ready));
	
	pthread_mutex_unlock(&(pool->task_lock));//释放锁

	return 1;

}

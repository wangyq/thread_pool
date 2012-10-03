#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include "thread_pool.h"
#include "cthread_pool.h"
#include "queue.h"

void* mytask(void* arg)
{
	int v = 0;
     printf ("threadid is 0x%x, working on task %ld\n", (int)pthread_self(),(long) arg);

     sleep (1);/*休息一秒，延长任务的执行时间*/

    PQueue pq = queue_init(sizeof(int) , 1,2);
    v=2; queue_push(pq,(void*)&v);
    v=0; queue_push(pq,(void*)&v);
    v=7; queue_push(pq,(void*)&v);
    v=9; queue_push(pq,(void*)&v);
    v=4; queue_push(pq,(void*)&v);
    v=8; queue_push(pq,(void*)&v);
    v=5; queue_push(pq,(void*)&v);

    while( !queue_is_empty(pq) ){
	     v = *((int*) queue_pop(pq));
	    printf("%d ", v);
    }
    printf("\n");

     sleep (1);/*休息一秒，延长任务的执行时间*/

	//must destroy!
     queue_destroy(pq);
     return NULL;
}


void * myprocess (void *arg)
{
	int v = 0; 
     printf ("threadid is 0x%x, working on task %d\n", (int)pthread_self(),*(int *) arg); 

     sleep (1);/*休息一秒，延长任务的执行时间*/ 

    PQueue pq = queue_init(sizeof(int) , 1,2);
    v=2; queue_push(pq,(void*)&v);
    v=0; queue_push(pq,(void*)&v);
    v=7; queue_push(pq,(void*)&v);
    v=9; queue_push(pq,(void*)&v);
    v=4; queue_push(pq,(void*)&v);
    v=8; queue_push(pq,(void*)&v);
    v=5; queue_push(pq,(void*)&v);
    
    while( !queue_is_empty(pq) ){
	     v = *((int*) queue_pop(pq));
	    printf("%d ", v);
    }
    printf("\n");
     
     sleep (1);/*休息一秒，延长任务的执行时间*/ 

	//must destroy!
     queue_destroy(pq);

    return NULL; 
} 


void test_threads()
{
    pool_init (3);/*线程池中最多三个活动线程*/

   sleep(1);

   /*连续向池中投入10个任务*/
   int *workingnum = (int *) malloc (sizeof (int) * 10);
   int i;
   for (i = 0; i < 10; i++)
    {
        workingnum[i] = i;
        pool_add_worker (myprocess, &workingnum[i]);
    }
   /*等待所有任务完成*/
    sleep (5);
   /*销毁线程池*/
    pool_destroy ();


    free (workingnum);
}

void test_pool()
{
	PThreadPool pool = thread_pool_init(3,0); //not lazy init!

	long i = 0;
	for(i=0;i<10;i++){
		thread_pool_add_task(pool, mytask, (void*)i);
	}

	/*等待所有任务完成*/
	sleep (5);
	thread_pool_destroy(pool);
}

int main (int argc, char **argv)
{ 
	//test_threads();
	test_pool();
    return 0; 
} 

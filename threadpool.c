#include "threadpool.h"
#include "pq.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <assert.h>

//Creates a new threadpool and returns a pointer to itself
threadpool_t *tp_create(int nthreads){

	//Make sure there is a valid number of threads
	if(nthreads > MAXTHREADS || nthreads < 1){
	    printf("ERROR: Invalid Number of Threads.\n");
	    exit(0);
	}
	
	//Intialize space for threadpool
	threadpool_t *pool = (threadpool_t *)malloc(sizeof(threadpool_t));
	if(!pool) exit(0);

	//Initialize priority queue for new threadpool
	pool->q = pq_create();
	if(!pool->q) exit(0);
	
	//Create nthreads number of threads to to tasks
	for(int i = 0; i < nthreads; i++){
		if(pthread_create(&pool->tid[i], NULL, looper, pool->q)) exit(0);
	}

	return pool;
}

//Submits tasks into the priority queue for worker threads to do
void tp_submit(threadpool_t *tp, void (*job)(void *), void *arg, short prio){
	
	//Creates space for task to be stored at
	task_t *t = (task_t*)malloc(sizeof(task_t));
	if(!t) exit(0);

	//Initialize t with job and arg
	t->point = job;
	t->argv = arg;

	//Insert t into the priority queue with prio priority
	pq_insert(tp->q, t, prio);

}

//Worker thread function
void* looper(void *q){
    
    //Arguements to get task from queue
    typedef void (*task_func_t)(void *);
    task_func_t task;
    task_t *get;
    void *arg;

    //Loop to get all of the tasks in the queue
    while(1){	
    	get = pq_next((pq_t*)q);

	task = get->point;
	arg = get->argv;

	task(arg);
    } 
}

#include "pq.h"

#ifndef THREADPOOL_H
#define THREADPOOL_H

#define MAXTHREADS 12   // arbitrary limit

//Task node strucuture
typedef struct{
	void *point;
	void *argv;
}task_t;

//Threadpool structure
typedef struct {  
	pq_t *q;
 	pthread_t tid[MAXTHREADS];	

} threadpool_t;

/* Create and return a pointer to an instance of the type
 * declared above.
 */
threadpool_t *tp_create(int nthreads);

/* Submit a job (task) to be executed by the given thread pool.
 * Jobs are functions that will be executed in a pool thread,
 * and passed the given parameter (arg).  Jobs are executed
 * in priority order, with higher priorities executed first.
 * Starvation of a low-priority job is possible if jobs with
 * higher priorities are submitted continually.
 */
void tp_submit(threadpool_t *tp, void (*job)(void *),
		void *arg, short prio);


void* looper(void *q);

#endif

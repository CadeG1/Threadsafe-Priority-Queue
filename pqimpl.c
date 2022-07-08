#include "pq.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


//Creates an new Priority queue and initializes
//the variables associated with the data structure
pq_t *pq_create(void){
	
	//Create space for the queue structure
	pq_t *q = (pq_t *)malloc(sizeof(pq_t));
	if(!q) return NULL;

	//Initialize starting node pointer to NULL
	q->start = NULL;

	//Initialize mutex and condition vairable
	if(pthread_mutex_init(&q->mx,NULL)) exit(0);
	if(pthread_cond_init(&q->cv,NULL)) exit(0);

	return q;
}


//Insert a new item into the priority queue given a 
//pointer to an item and a given priority
void pq_insert(pq_t *q, void *item, short prio){	

	//Initialiszes pointer the queues currrent starting node
	Node* curr = q->start;

	//Initializes memeory for new node to be isnerted to
	Node* temp = (Node *)malloc(sizeof(Node));
	if(!temp) exit(0);

	//Gives new node given parameters
	temp->self = item;
	temp->prio = prio;
	temp->next = NULL;
	
	//If no starting node, this becomes the starting node,
	//else traverse queue to find where to place the new node
	if(q->start == NULL){
		q->start = temp;
	}
	else{
       	   if(curr->prio < prio){
		temp->next = q->start;
		q->start = temp;
	
	   }  
	   else{
	 	while((curr->next != NULL) && curr->next->prio >= prio){
		curr = curr->next;
		}
		
		temp->next = curr->next;
		curr->next = temp;	
	}
    } 
   
    //Signal one (1) waiting threads that queue has been updated
    if(pthread_cond_signal(&q->cv)) exit(0);
 
}

//Get the current highest node in the queue and return the item
//in the node, along with freeing the that node's memory from the queue
void *pq_next(pq_t *q){

	//Lock other threads from interfering
	if(pthread_mutex_lock(&q->mx)) exit(0);

	//Wait for there to be a node in the thread to do work on
	while(q->start == NULL){
	       if(pthread_cond_wait(&q->cv, &q->mx)) exit(0);
	}

	//Change the top of the queue to the next highest node
	Node *temp = q->start;
	void *ret = temp->self;
	(q->start) = temp->next;

	//Free next node's memory 
	free(temp);
        
	//Unlock waiting threads
	if(pthread_mutex_unlock(&q->mx)) exit(0);

	return ret;
}


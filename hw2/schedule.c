/* schedule.c
 * This file contains the primary logic for the
 * scheduler.
 */
#include "schedule.h"
#include "macros.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "privatestructs.h"

#define NEWTASKSLICE (NS_TO_JIFFIES(100000000))
#define AGEING 0.5

/* Local Globals
 * rq - This is a pointer to the runqueue that the scheduler uses.
 * current - A pointer to the current running task.
 */
struct runqueue *rq;
struct task_struct *current;

/* External Globals
 * jiffies - A discrete unit of time used for scheduling.
 *			 There are HZ jiffies in a second, (HZ is
 *			 declared in macros.h), and is usually
 *			 1 or 10 milliseconds.
 */
extern long long jiffies;
extern struct task_struct *idle;

/*-----------------Initilization/Shutdown Code-------------------*/
/* This code is not used by the scheduler, but by the virtual machine
 * to setup and destroy the scheduler cleanly.
 */

 /* initscheduler
  * Sets up and allocates memory for the scheduler, as well
  * as sets initial values. This function should also
  * set the initial effective priority for the "seed" task
  * and enqueu it in the scheduler.
  * INPUT:
  * newrq - A pointer to an allocated rq to assign to your
  *			local rq.
  * seedTask - A pointer to a task to seed the scheduler and start
  * the simulation.
  */

void initschedule(struct runqueue *newrq, struct task_struct *seedTask)
{
    seedTask->thread_info->burst=0;
    seedTask->thread_info->exp_burst=0;
	seedTask->next = seedTask->prev = seedTask;
	newrq->head = seedTask;
	newrq->nr_running++;
    printf("locatin of init %p\n", seedTask);

}

/* killschedule
 * This function should free any memory that
 * was allocated when setting up the runqueu.
 * It SHOULD NOT free the runqueue itself.
 */
void killschedule(){

	/*struct task_struct *tmp, *curr;
	struct thread_info *thread;
	curr = rq->head;

	while(curr->next != NULL){
		curr = curr -> next;
	}

	while(curr->prev != NULL){

		thread = curr->thread_info;
		printf("Freed processes with Id: %d\n", thread->id);
		tmp = curr->prev;
		free(thread->type_struct);
		free(thread->processName);
		free(thread->parent);
		free(thread);
		free(curr);
		curr = tmp;
	}
	thread = curr->thread_info;
	printf("Freed processes with Id: %d\n", thread->id);
	//free(thread->type_struct);
	//free(thread->processName);
	//free(thread->parent);
	//free(thread);
	free(curr);
    if (rq->head==NULL){
        printf("Noting in head of rq\n");
    }else{
        
        printf("my ponter location is %p\n", rq->head);
        free(rq->head);

    }*/
	return;
}


void print_rq () {
	struct task_struct *curr;

	printf("Rq: \n");
	curr = rq->head;
	if (curr)
		printf("%p", curr);
	while(curr->next != rq->head) {
		curr = curr->next;
		printf(", %p", curr);
	};
	printf("\n");
}

/*-------------Scheduler Code Goes Below------------*/
/* This is the beginning of the actual scheduling logic */

/* schedule
 * Gets the next task in the queue
 */
void schedule()
{
	static struct task_struct *nxt = NULL;
	struct task_struct *curr, *test;
    long long start_burst, end_burst;

    //printf("In schedule\n");
    //print_rq();

	current->need_reschedule = 0; /* Always make sure to reset that, in case *
								   * we entered the scheduler because current*
								   * had requested so by setting this flag   */

    curr=NULL;
	
    if (rq->nr_running == 1) {
        context_switch(rq->head);
		nxt = rq->head->next;
	}
	else {
		curr = nxt;
		nxt = nxt->next;
		if (nxt == rq->head)    /* Do this to always skip init at the head */
			nxt = nxt->next;	/* of the queue, whenever there are other  */
								/* processes available					   */

        start_burst=sched_clock();
		context_switch(curr);
        end_burst=sched_clock();
	}

    
    printf("times start: %lld end: %lld\n", start_burst, end_burst);
    if(curr!=NULL){
        curr->thread_info->exp_burst=( (curr->thread_info->burst + ( AGEING*curr->thread_info->exp_burst ) ) / (1+AGEING) );
        curr->thread_info->burst=end_burst-start_burst;
    }
    
    test=nxt;
    while(test!=NULL){
        
        if(test->thread_info->id==0){
            printf("Hi iam the init my burst:%lld exp_burst: %lld\n", test->thread_info->burst, test->thread_info->exp_burst);
            printf("My prev %d\n", test->prev->thread_info->id);
            break;
        }
        printf("num of process in runqueu %lu id of process: %d processName %s my burst: %lld my exp_burst: %lld\n",rq->nr_running, test->thread_info->id, test->thread_info->processName, test->thread_info->burst, test->thread_info->exp_burst);
        test=test->next;


    }
      
}


/* sched_fork
 * Sets up schedule info for a newly forked task
 */
void sched_fork(struct task_struct *p)
{
	p->time_slice = 100000000;
    p->thread_info->burst=0;
    p->thread_info->exp_burst=0;
    printf("Hello i am processName: %s and my id: %d\n", p->thread_info->processName, p->thread_info->id);
    if(p->next!=NULL){

        printf("Hello i am the parent : %s and my id: %d\n", p->next->thread_info->processName, p->next->thread_info->id);

    }
}

/* scheduler_tick
 * Updates information and priority
 * for the task that is currently running.
 */
void scheduler_tick(struct task_struct *p)
{
	schedule();
}

/* wake_up_new_task
 * Prepares information for a task
 * that is waking up for the first time
 * (being created).
 */
void wake_up_new_task(struct task_struct *p)
{
	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;

	rq->nr_running++;
}

/* activate_task
 * Activates a task that is being woken-up
 * from sleeping.
 */
void activate_task(struct task_struct *p)
{
	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;

	rq->nr_running++;
}

/* deactivate_task
 * Removes a running task from the scheduler to
 * put it to sleep.
 */
void deactivate_task(struct task_struct *p)
{
	p->prev->next = p->next;
	p->next->prev = p->prev;
	p->next = p->prev = NULL; /* Make sure to set them to NULL *
							   * next is checked in cpu.c      */

	rq->nr_running--;
}

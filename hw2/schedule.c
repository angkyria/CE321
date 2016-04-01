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

void initschedule(struct runqueue *newrq, struct task_struct *seedTask){

    seedTask->burst=0;
	seedTask->start_burst=0;
	seedTask->end_burst=0;
    seedTask->exp_burst=0;
    seedTask->CPU=0;
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
void schedule(){

    struct task_struct *curr, *min, *max, *min1;
    unsigned long long curr_time;


    if(current->CPU ==0){

        if(current!=rq->head){
           // start_burst = sched_clock();
            current->CPU = 1;
        }

    }

    current->need_reschedule = 0; /* Always make sure to reset that, in case *
                                   * we entered the scheduler because current*
                                   * had requested so by setting this flag   */

    curr=NULL;

    if (rq->nr_running == 1) {
        context_switch(rq->head);
        current = rq->head->next;

    }else{
        if (current == rq->head) {
            current = current->next;
        }

        curr = rq->head->next;
        min = curr;

        while (curr != rq->head) {                     // finds minimum exp burst
            if (min->exp_burst > curr->exp_burst) {
                min = curr;
            }
            curr = curr->next;
        }


        curr = rq->head->next;
        max=curr;

        curr_time = sched_clock();
        while (curr != rq->head) {
            curr->waitingRQ=curr_time - curr->entry_time_RQ;
            if (max->waitingRQ < curr->waitingRQ) {
                max=curr;
            }
            curr = curr->next;
        }

        current->end_burst=curr_time;

        current->exp_burst=( (current->burst + ( AGEING*current->exp_burst ) ) / (1+AGEING) );

	    current->burst=current->end_burst-current->start_burst;

        //current->goodness = ((1 + current->exp_burst)/(1 + min->exp_burst))*((1 + max->waitingRQ)/(1 + current->waitingRQ));


        curr = rq->head->next;
        min1 = curr;

        while (curr != rq->head) {                 // finds minimum goodness
            curr->goodness= ((1 + curr->exp_burst)/(1 + min->exp_burst))*((1 + max->waitingRQ)/(1 + curr->waitingRQ)); 
            if (min1->goodness > curr->goodness) {
                min1 = curr;
            }
            curr = curr->next;
        }

        if (min1 != current) {
			min1->start_burst = sched_clock();
			//current->end_burst = sched_clock();
            current->CPU = 0;
            printf("____________________\n");
			printf("(%s) start_burst: %lld end_burst:%lld burst: %lld ext_burst:%lld waitingRQ: %lld goodness: %lld\n",current->thread_info->processName, current->start_burst, current->end_burst, current->burst, current->exp_burst, current->waitingRQ, current->goodness);
			curr = rq->head->next;
			printf("\n---------------------\n");
			printf("|        RQ         |\n");
			printf("---------------------\n");
			while (curr!= rq->head) {
				 printf("(%s): start_burst: %lld end_burst:%lld burst: %lld ext_burst:%lld waitingRQ: %lld goodness: %lld \n",curr->thread_info->processName, curr->start_burst, curr->end_burst, curr->burst, curr->exp_burst, curr->waitingRQ, curr->goodness);
				 curr=curr->next;
			}
			printf("|                   |\n");
			printf("---------------------\n");
			context_switch(min1);
        }
    }
}


/* sched_fork
 * Sets up schedule info for a newly forked task
 */
void sched_fork(struct task_struct *p)
{
    p->time_slice = 0;
    p->burst=0;
    p->exp_burst=0;
    p->goodness=0;
    p->CPU=0;
	p->start_burst = 0;
	p->end_burst=0;
    p->waitingRQ=0;

}

/* scheduler_tick
 * Updates information and priority
 * for the task that is currently running.
 */
void scheduler_tick(struct task_struct *p){

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
    p->entry_time_RQ = sched_clock();
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
    p->entry_time_RQ = sched_clock();


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

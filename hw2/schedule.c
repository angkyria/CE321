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
}

/* killschedule
 * This function should free any memory that
 * was allocated when setting up the runqueu.
 * It SHOULD NOT free the runqueue itself.
 */
void killschedule(){
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

    struct task_struct *curr, *min_exp_burst, *max_waiting, *min_goodness;
    unsigned long long curr_time;


    if(current->CPU ==0){

        if(current!=rq->head){
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
        min_exp_burst = curr;

        while (curr != rq->head) {                     /* finds minimum exp burst*/
            if (min_exp_burst->exp_burst > curr->exp_burst) {
                min_exp_burst = curr;
            }
            curr = curr->next;
        }


        curr = rq->head->next;
        max_waiting=curr;

        curr_time = sched_clock();
        while (curr != rq->head) {                  /*finds max waiting time in rq*/
            if(curr!=current){

                curr->waitingRQ=curr_time - curr->entry_time_RQ;
            }
            if (max_waiting->waitingRQ < curr->waitingRQ) {
                max_waiting=curr;
            }
            curr = curr->next;
        }

        current->end_burst=curr_time;

        current->exp_burst=( (current->burst + ( AGEING*current->exp_burst ) ) / (1+AGEING) );

	    current->burst=current->end_burst-current->start_burst;


        curr = rq->head->next;
        min_goodness = curr;

        while (curr != rq->head) {                 /* finds minimum goodness*/
            curr->goodness= ((1 + curr->exp_burst)/(1 + min_exp_burst->exp_burst))*((1 + max_waiting->waitingRQ)/(1 + curr->waitingRQ));
            if (min_goodness->goodness > curr->goodness) {
                min_goodness = curr;
            }
            curr = curr->next;
        }

        if (min_goodness != current) {
			min_goodness->start_burst = sched_clock();
            current->CPU = 0;
            printf("____________________\n");
            printf("max_waiting: %lld, min_exp_burst: %lld\n", max_waiting->waitingRQ, min_exp_burst->exp_burst);
			printf("(%d) curr_time %lld ext_burst:%lld  waitingRQ:%lld goodness: %lld\n",current->thread_info->id, curr_time, current->exp_burst, current->waitingRQ, current->goodness);
			curr = rq->head->next;
			printf("\n---------------------\n");
			printf("|        RQ         |\n");
			printf("---------------------\n");
			while (curr!= rq->head) {
				 printf("(%d): ext_burst:%lld waitingRQ: %lld goodness: %lld \n",curr->thread_info->id, curr->exp_burst, curr->waitingRQ, curr->goodness);
				 curr=curr->next;
			}
			printf("|                   |\n");
			printf("---------------------\n");
			context_switch(min_goodness);
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

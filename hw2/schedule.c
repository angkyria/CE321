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
unsigned long long start_burst=0, end_burst=0;

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

    static struct task_struct *nxt = NULL;
    struct task_struct *curr, *test, *tmp, *min, *curr2, *max;
    unsigned long long max_wait, min_exp_burst, currTime;
    //long long start_burst, end_burst;

    //printf("In schedule\n");
    //print_rq();
    if (current != rq->head) {
        printf("-------------------------------\n");
        printf("{{TIMES START %lld END: %lld\n", start_burst, end_burst);
        printf("-------------------------------\n");
    }
    if(current->CPU ==0){

        if(current!=rq->head){
            start_burst = sched_clock();
            current->CPU = 1;
        }

    }

    current->need_reschedule = 0; /* Always make sure to reset that, in case *
                                   * we entered the scheduler because current*
                                   * had requested so by setting this flag   */

    curr=NULL;

    if (rq->nr_running == 1) {
        context_switch(rq->head);
        nxt = rq->head->next;

    }else{
        if (current == rq->head) {
            current = current->next;
        }
        /*curr = nxt;
         nxt = nxt->next;
         if (nxt == rq->head)    /* Do this to always skip init at the head
         nxt = nxt->next;	/* of the queue, whenever there are other
         /* processes available
         */
        /**********/

        curr2 = rq->head->next;
        min = curr2;
        tmp = rq->head->next;

        while (curr2 != rq->head) {                     // finds minimum exp burst
            if (min->exp_burst > curr2->exp_burst) {
                min = curr2;
            }
            curr2 = curr2->next;
        }

        min_exp_burst = min->exp_burst;

        curr2 = rq->head->next;
        max = curr2;
        tmp = rq->head->next;

        currTime = sched_clock();
        max_wait = currTime - max->entry_time_RQ;
        while (curr2 != rq->head) {
            if (currTime - max->entry_time_RQ < currTime - curr2->entry_time_RQ) {
                max_wait = currTime - curr2->entry_time_RQ;
            }
            curr2 = curr2->next;
        }


        /*else {
            printf("[[CURR Hi iam the current(%s) process my burst:%lld exp_burst: %lld goodness: %lld\n",current->thread_info->processName, current->burst, current->exp_burst, current->goodness);
            context_switch(current);
        }*/

        end_burst=sched_clock();
        current->burst=end_burst-start_burst;
        current->exp_burst=( (current->burst + ( AGEING*current->exp_burst ) ) / (1+AGEING) );

        current->goodness = ((1 + current->exp_burst)/(1 + min->exp_burst))*((1 + max_wait)/(1 + currTime - current->entry_time_RQ));
        printf("[[Hi iam the current(%s) process my burst:%lld exp_burst: %lld goodness: %lld\n",current->thread_info->processName, current->burst, current->exp_burst, current->goodness);

        curr2 = rq->head->next;
        min = curr2;
        tmp = rq->head->next;

        while (curr2 != rq->head) {                 // finds minimum goodness
            if (min->goodness > curr2->goodness) {
                min = curr2;
            }
            curr2 = curr2->next;
        }

        if (min != current) {
            current->CPU = 0;

        }

        context_switch(min);


        /**********/
        /*context_switch(curr); */

    }




    /*test=rq->head->next;

     while(test!=NULL){

     if(test->thread_info->id==0){
     printf("||Hi iam the init my burst:%lld exp_burst: %lld\n", test->burst, test->exp_burst);
     printf("||My prev %d\n", test->prev->thread_info->id);
     break;
     }
     printf("||num of process in runqueu %lu id of process: %d processName %s my burst: %lld my exp_burst: %lld\n",rq->nr_running, test->thread_info->id, test->thread_info->processName, test->burst, test->exp_burst);
     test=test->next;

     }*/

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
    /*printf("Hello i am processName: %s and my id: %d\n", p->thread_info->processName, p->thread_info->id);
     if(p->next!=NULL){

     printf("Hello i am the parent : %s and my id: %d\n", p->next->thread_info->processName, p->next->thread_info->id);

     }*/

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

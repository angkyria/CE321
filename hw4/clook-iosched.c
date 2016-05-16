/*
 * elevator clook
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

int direction = 1;
struct request *temp_rq = NULL;

struct clook_data {
	struct list_head queue;
};

static void clook_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

static int clook_dispatch(struct request_queue *q, int force)
{
	struct clook_data *nd = q->elevator->elevator_data;
	char action;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);

        if( rq == list_last_entry(&nd->queue, struct request, queuelist) && direction == 1){
            direction = -direction;
			printk("\nRight -> Left");
        }
		if( rq == list_first_entry(&nd->queue, struct request, queuelist) && direction == -1){
			direction = -direction;
			printk("\nLeft -> Right");
		}

		temp_rq = rq; 	/*last position */


		if(rq_data_dir(rq) == WRITE){
			action = 'W';
		}else{
			action = 'R';
		}
		printk("[CLOOK] dsp %c %llu \n", action, blk_rq_pos(rq) );

		return 1;
	}
	return 0;
}

static void clook_add_request(struct request_queue *q, struct request *rq)
{
	char action;
	//struct list_head *curr=NULL;
	struct request *curr_req=NULL;

	struct clook_data *nd = q->elevator->elevator_data;
	if (list_empty(&nd->queue)) {
		list_add_tail(&rq->queuelist, &nd->queue);
		printk("----Added first request \n");

			if(rq_data_dir(rq) == WRITE){
				action = 'W';
			}else{
				action = 'R';
			}
			printk("[CLOOK] add %c %llu \n", action, blk_rq_pos(rq));
			return ;
	}

	if (blk_rq_pos(rq) > blk_rq_pos(temp_rq)) {

		list_for_each_entry(curr_req, &temp_rq->queuelist, queuelist){

			if(blk_rq_pos(curr_req)<blk_rq_pos(rq))
				continue;

			break;
		}

	}else{

		list_for_each_entry_reverse(curr_req, &temp_rq->queuelist, queuelist){

			if(blk_rq_pos(curr_req)>blk_rq_pos(rq))
				continue;

			break;
		}


	}

	list_add_tail(&rq->queuelist, &curr_req->queuelist);

	if(rq_data_dir(rq) == WRITE){
		action = 'W';
	}else{
		action = 'R';
	}
	printk("[CLOOK] add %c %llu \n", action, blk_rq_pos(rq));

	/*list_add_tail(&rq->queuelist, &nd->queue);*/
}

static struct request *
clook_former_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;

	printk(" ----------- This is clook_former_request ----------\n");

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
clook_latter_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;
	printk(" ----------- This is clook_latter_request ----------\n");
	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int clook_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct clook_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);
	printk("-------Init queue--------- \n");

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void clook_exit_queue(struct elevator_queue *e)
{
	struct clook_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_clook = {
	.ops = {
		.elevator_merge_req_fn		= clook_merged_requests,
		.elevator_dispatch_fn		= clook_dispatch,
		.elevator_add_req_fn		= clook_add_request,
		.elevator_former_req_fn		= clook_former_request,
		.elevator_latter_req_fn		= clook_latter_request,
		.elevator_init_fn		= clook_init_queue,
		.elevator_exit_fn		= clook_exit_queue,
	},
	.elevator_name = "clook",
	.elevator_owner = THIS_MODULE,
};

static int __init clook_init(void)
{
	return elv_register(&elevator_clook);
}

static void __exit clook_exit(void)
{
	elv_unregister(&elevator_clook);
}

module_init(clook_init);
module_exit(clook_exit);


MODULE_AUTHOR("Team 16");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Clook IO scheduler");

2c2,11
<  * elevator noop
---
>  * elevator look
>  *
>  * CS411 Team 11
>  *  Ben Kero
>  *  Clint Whiteside
>  *  Michael Johnson
>  *
>  * What we changed:
>  * Everything
>  *
10c19,21
< struct noop_data {
---
> /* Added *next, *prev to keep track of where we are in the queue */
> struct look_data {
> 	struct list_head *prev;
11a23
> 	struct list_head *next;
14c26,38
< static void noop_merged_requests(struct request_queue *q, struct request *rq,
---
> /*
>  * Which direction are we going?
>  * 0 -> Low to high
>  * 1 -> High to low
>  *
>  */
>
> bool direction=0;
> unsigned long maxSector = 0;
> unsigned long minSector = 1000000;
>
> /* I don't believe we need to modify this */
> static void look_merged_requests(struct request_queue *q, struct request *rq,
20c44,45
< static int noop_dispatch(struct request_queue *q, int force)
---
> /* Modified to use our pointer which points to the next request */
> static int look_dispatch(struct request_queue *q, int force)
22c47,57
< 	struct noop_data *nd = q->elevator->elevator_data;
---
>   struct look_data *nd = q->elevator->elevator_data;
>
>   if (!list_empty(&nd->queue)) {
>     struct request *nextitem = list_entry(nd->next, struct request, queuelist);
>     printk("[look] dsp %s %lu\n", (nextitem->cmd_flags & REQ_RW) ? "write" : "read", (unsigned long)nextitem->sector);
>     nd->next = nextitem->queuelist.next; //Update our next pointer
>     list_del_init(&nextitem->queuelist);
>     elv_dispatch_sort(q, nextitem);
>     return 1;
>   }
>   return 0;
24,31d58
< 	if (!list_empty(&nd->queue)) {
< 		struct request *rq;
< 		rq = list_entry(nd->queue.next, struct request, queuelist);
< 		list_del_init(&rq->queuelist);
< 		elv_dispatch_sort(q, rq);
< 		return 1;
< 	}
< 	return 0;
34c61,62
< static void noop_add_request(struct request_queue *q, struct request *rq)
---
> /* This function takes new requests and puts them where they need to be */
> static void look_add_request(struct request_queue *q, struct request *rq)
36c64,90
< 	struct noop_data *nd = q->elevator->elevator_data;
---
>   struct look_data *nd = q->elevator->elevator_data;
>   struct list_head *pos, *insert_spot = 0;
>   struct request *queueitem;
>
>   printk("[look] add %s %lu\n", (rq->cmd_flags & REQ_RW) ? "write" : "read", (unsigned long)rq->sector);
>
>   /* empty list, we just add and update the poiter */
>   if (list_empty(&nd->queue)) {
>     nd->next = &rq->queuelist;
>     list_add_tail(&rq->queuelist, &nd->queue);
>     return;
>   }
>
>   /* iterate backwards for simplicity
>  *    * if we don't find any match, we insert at the beginning
>  *       */
>   list_for_each_prev(pos, &nd->queue) {
>     queueitem = list_entry(pos, struct request, queuelist);
>     insert_spot = pos;
>     if (queueitem->sector < rq->sector) {
>       break;
>     }
>   }
>
>   /* add the request */
>   list_add(&rq->queuelist, insert_spot);
>
38d91
< 	list_add_tail(&rq->queuelist, &nd->queue);
41c94,95
< static int noop_queue_empty(struct request_queue *q)
---
> /* No need to modify this */
> static int look_queue_empty(struct request_queue *q)
43c97
< 	struct noop_data *nd = q->elevator->elevator_data;
---
> 	struct look_data *nd = q->elevator->elevator_data;
47a102
> /* Probably don't need to modify this */
49c104
< noop_former_request(struct request_queue *q, struct request *rq)
---
> look_former_request(struct request_queue *q, struct request *rq)
51c106
< 	struct noop_data *nd = q->elevator->elevator_data;
---
> 	struct look_data *nd = q->elevator->elevator_data;
57a113
> /* Probably don't need to modify this */
59c115
< noop_latter_request(struct request_queue *q, struct request *rq)
---
> look_latter_request(struct request_queue *q, struct request *rq)
61c117
< 	struct noop_data *nd = q->elevator->elevator_data;
---
> 	struct look_data *nd = q->elevator->elevator_data;
68c124,125
< static void *noop_init_queue(struct request_queue *q)
---
> /* Sets up the queue, no modification required */
> static void *look_init_queue(struct request_queue *q)
70c127
< 	struct noop_data *nd;
---
> 	struct look_data *nd;
79c136,137
< static void noop_exit_queue(elevator_t *e)
---
> /* No changes necessary */
> static void look_exit_queue(elevator_t *e)
81c139
< 	struct noop_data *nd = e->elevator_data;
---
> 	struct look_data *nd = e->elevator_data;
87c145
< static struct elevator_type elevator_noop = {
---
> static struct elevator_type elevator_look = {
89,96c147,154
< 		.elevator_merge_req_fn		= noop_merged_requests,
< 		.elevator_dispatch_fn		= noop_dispatch,
< 		.elevator_add_req_fn		= noop_add_request,
< 		.elevator_queue_empty_fn	= noop_queue_empty,
< 		.elevator_former_req_fn		= noop_former_request,
< 		.elevator_latter_req_fn		= noop_latter_request,
< 		.elevator_init_fn		= noop_init_queue,
< 		.elevator_exit_fn		= noop_exit_queue,
---
> 		.elevator_merge_req_fn		= look_merged_requests,
> 		.elevator_dispatch_fn		= look_dispatch,
> 		.elevator_add_req_fn		= look_add_request,
> 		.elevator_queue_empty_fn	= look_queue_empty,
> 		.elevator_former_req_fn		= look_former_request,
> 		.elevator_latter_req_fn		= look_latter_request,
> 		.elevator_init_fn		= look_init_queue,
> 		.elevator_exit_fn		= look_exit_queue,
98c156
< 	.elevator_name = "noop",
---
> 	.elevator_name = "look",
102c160
< static int __init noop_init(void)
---
> static int __init look_init(void)
104c162
< 	return elv_register(&elevator_noop);
---
> 	return elv_register(&elevator_look);
107c165
< static void __exit noop_exit(void)
---
> static void __exit look_exit(void)
109c167
< 	elv_unregister(&elevator_noop);
---
> 	elv_unregister(&elevator_look);
112,113c170,171
< module_init(noop_init);
< module_exit(noop_exit);
---
> module_init(look_init);
> module_exit(look_exit);
116c174
< MODULE_AUTHOR("Jens Axboe");
---
> MODULE_AUTHOR("CS411 Team 11, The Fighting Walruses");
118c176
< MODULE_DESCRIPTION("No-op IO scheduler");
---
> MODULE_DESCRIPTION("LOOK IO scheduler");

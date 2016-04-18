#include<linux/kernel.h>
#include<linux/syscalls.h>
#include<linux/list.h>

static LIST_HEAD(free_slob_small);
static LIST_HEAD(free_slob_medium);
static LIST_HEAD(free_slob_large);

SYSCALL_DEFINE0(slob_get_total_free_mem) {
	
}

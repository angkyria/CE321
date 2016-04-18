#include<linux/kernel.h>
#include<linux/syscalls.h>
#include<linux/slab.h>
#include<linux/mm.h>

SYSCALL_DEFINE0(slob_get_total_alloc_mem) {

	return(total_alloc_mem);

}

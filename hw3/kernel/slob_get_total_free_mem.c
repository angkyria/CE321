#include<linux/kernel.h>
#include<linux/syscalls.h>
#include<linux/slab.h>
#include <linux/mm.h>

SYSCALL_DEFINE0(slob_get_total_free_mem) {

	//long int free_mem = total_free_mem;

	return(total_free_mem);
}

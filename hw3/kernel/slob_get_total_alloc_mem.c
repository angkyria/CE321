#include<linux/kernel.h>
#include<linux/syscalls.h>
#include<linux/slab.h>

SYSCALL_DEFINE0(slob_get_total_alloc_mem) {

	long allocated_mem[100];
	long allocated_mem_avg = 0;
	int i;

	for (i = 0; i < 100; i++) {
		allocated_mem[i] = total_alloc_mem;
	}

	for(i=0; i < 100; i++){
		allocated_mem_avg = allocated_mem_avg + allocated_mem[i];
	}

	allocated_mem_avg = allocated_mem_avg / 100;

	return(allocated_mem_avg);

}

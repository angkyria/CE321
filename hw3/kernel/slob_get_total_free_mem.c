#include<linux/kernel.h>
#include<linux/syscalls.h>
#include<linux/slab.h>

SYSCALL_DEFINE0(slob_get_total_free_mem) {

	long emty_mem[100];
	long emty_mem_avg = 0;
	int i;

	for (i = 0; i < 100; i++) {
		emty_mem[i] = total_free_mem;
	}

	for(i=0; i < 100; i++){
		emty_mem_avg = emty_mem_avg + emty_mem[i];
	}

	emty_mem_avg = emty_mem_avg / 100;

	return(emty_mem_avg);
}

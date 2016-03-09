#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <asm/current.h>

SYSCALL_DEFINE0(find_roots) {

	struct task_struct *curr;
	curr=current;
	
	printk("find_roots system call called by process %d\n", curr->pid);
	
	do{
		printk("id: %d, name: %s\n", curr->pid, curr->comm);
		curr=curr->parent;
	}while(curr->pid != 1);
	
	printk("id: %d, name: %s\n", curr->pid, curr->comm);

	return 0;

}

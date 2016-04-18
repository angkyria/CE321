#include <sys/syscall.h>
#include <unistd.h>
#include <slob_mem_measurement.h>

long slob_get_total_alloc_mem(void){
	return( syscall(__NR_slob_get_total_alloc_mem));
}

#include <stdio.h>
#include "slob_mem_measurement.h"

int main(){
	printf(" %ld", slob_get_total_alloc_mem());
	printf(" %ld", slob_get_total_free_mem());

	return 0;
}

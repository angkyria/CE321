#include <stdio.h>
#include "slob_mem_measurement.h"

int main(){

	printf("Average memory allocated: %ld \n",slob_get_total_alloc_mem());
	printf("Average memory not in use: %ld \n", slob_get_total_free_mem());

	return 0;
}

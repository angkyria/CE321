#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main( int argc, char *argv[] ){

	int *ptr;
	ptr = (int*)malloc(sizeof(int)*2500);

	char *buffer0, *buffer1, *buffer2, *buffer3, *buffer4, *buffer7;

        buffer0 = (char*) malloc (sizeof(char) *9000000);
        buffer1 = (char*) malloc (sizeof(char) *345555);
        buffer2 = (char*) malloc (sizeof(char) *1234);
        buffer3 = (char*) malloc (sizeof(char) *144445554);
        buffer4 = (char*) malloc (sizeof(char) *55630);
        buffer7 = (char*) malloc (sizeof(char) *1000000000);

	for( int i=0; i < 1000; i++){

		ptr = (int*)realloc(ptr,(2500 * i) );
		
		sleep(1);

	}


	return 0;
}

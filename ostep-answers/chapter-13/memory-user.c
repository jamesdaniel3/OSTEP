#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char * argv[]){
	if (argc != 2) {
		printf("Bad args!");
		exit(1);
	}

	unsigned long long arr_size = strtod(argv[1], NULL) * 1000000; // convert megabytes to bytes
	char * arr = calloc(arr_size, 1);

	while (true) {
		for(unsigned long long i = 0; i < arr_size; i++) {
			arr[i]++;
		}
	}

	return 0;
}
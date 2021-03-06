#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fsmb_largefile.h"
#include "fsmb_smallfile.h"

enum fsmb_type{
	FSMB_TYPE_LARGE_FILE = 0,
	FSMB_TYPE_SMALL_FILE = 1
};

void main(int argc, const char * argv[]){
	/* 1. argument parse phase */
	int option;		// large or small
	int block_size;	// byte unit
	int iter_count;
	char * log_filename;

	if(argc != 6){
		printf("Usage: fsmb [large|small] block_byte_size filepath iteration log_filename\n");
		exit(1);
	}

	if(strcmp(argv[1], "large") == 0){
		option = FSMB_TYPE_LARGE_FILE;
	} else if (strcmp(argv[1], "small") == 0){
		option = FSMB_TYPE_SMALL_FILE;
	} else {
		printf("Wrong option (use large, small)\nUsage: fsmb [large|small] block_byte_size filepath iteration log_filename\n");
		exit(1);
	}

	block_size = atoi(argv[2]);
	if(block_size == 0){
		printf("Wrong block size\nUsage: fsmb [large|small] block_byte_size filepath iteration log_filename\n");
		exit(1);
	}

	// argv[3] is filepath

	iter_count = atoi(argv[4]);
	log_filename = argv[5];

	/* 2. execute microbenchmark phase */
	if(option == FSMB_TYPE_LARGE_FILE){
		fsmb_largefile_benchmark(argv[3], block_size, iter_count, log_filename);
	} else if (option == FSMB_TYPE_SMALL_FILE){
		fsmb_smallfile_benchmark(argv[3], block_size, iter_count, log_filename);
	}
}

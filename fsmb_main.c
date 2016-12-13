#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fsmb_largefile.h"
#include "fsmb_smallfile.h"
#include "fsmb_iocall.h"

enum fsmb_type{
	FSMB_TYPE_LARGE_FILE = 0,
	FSMB_TYPE_SMALL_FILE = 1,
	FSMB_TYPE_IO_CALL = 2
};

void main(int argc, const char * argv[]){
	/* 1. argument parse phase */
	int option;		// large or small
	int block_size;	// byte unit
	int iter_count;

	if(argc != 5){
		printf("Usage: fsmb [large|small|iocall] block_byte_size filepath iteration\n");
		exit(1);
	}

	if(strcmp(argv[1], "large") == 0){
		option = FSMB_TYPE_LARGE_FILE;
	} else if (strcmp(argv[1], "small") == 0){
		option = FSMB_TYPE_SMALL_FILE;
	} else if (strcmp(argv[1], "iocall") == 0){
		option = FSMB_TYPE_IO_CALL;
	} else {
		printf("Wrong option (use large, small or iocall)\nUsage: fsmb [large|small|iocall] block_byte_size filepath iteration\n");
		exit(1);
	}

	block_size = atoi(argv[2]);
	if(block_size == 0){
		printf("Wrong block size\nUsage: fsmb [large|small|iocall] block_byte_size filepath iteration\n");
		exit(1);
	}

	// argv[3] is filepath

	iter_count = atoi(argv[4]);


	/* 2. execute microbenchmark phase */
	if(option == FSMB_TYPE_LARGE_FILE){
		fsmb_largefile_benchmark(argv[3], block_size, iter_count);
	} else if (option == FSMB_TYPE_SMALL_FILE){
		fsmb_smallfile_benchmark(argv[3], block_size, iter_count);
	} else if (option == FSMB_TYPE_IO_CALL){
		fsmb_io_system_call_benchmark(argv[3], block_size, iter_count);
	}
}

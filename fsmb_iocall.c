#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "time_util.h"
#include "fsmb_iocall.h"

#define BUF_SIZE 1024*1024 // 1mb

void fsmb_io_system_call_benchmark(const char * filepath, int block_size, int count){
	int fd;
	int ret_val;
	char buf[BUF_SIZE];

	for(int i=0;i<BUF_SIZE;i++){
		buf[i] = i % 2;
	}

	fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		perror("Failed to open file\n");
		exit(1);
	}

	printf("%d bytes writing time\n", block_size);
	for(int iter=0;iter<count;iter++){
		TIMER_START();

		ret_val = write(fd, buf, block_size);
		if(ret_val != block_size){
			printf("failed to write a whole block\n");
			exit(1);
		}

		fsync(fd);

		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		printf("%ld.%09ld\n", sec, nsec);
	}

	//rewind file pointer
	ret_val = lseek(fd, 0, SEEK_SET);
	if(ret_val<0){
		perror("failed to lseek");
		exit(1);
	}

	printf("%d bytes reading time\n", block_size);
	for(int iter=0;iter<count;iter++){
		TIMER_START();

		ret_val = read(fd, buf, block_size);
		if(ret_val != block_size){
			printf("failed to read a whole block\n");
			exit(1);
		}

		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		printf("%ld.%09ld\n", sec, nsec);
	}

	unlink(filepath);
}
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "fsmb_largefile.h"
#include "time_util.h"

/*
 * Largefile microbenchmark Design
 *
 * 1. create/open/write 1 block(various length)
 *    repetitively until reaching the pre-defined file size.
 * 2. read sequentially block-by-block
 * 3. lseek() random access, read, rewrite 1 block
 * 4. delete file
 *
 */

//TODO: make iteration

void fsmb_largefile_benchmark(const char * filepath, int block_size, int count){
	int fd, wrt_size, rd_size;
	char buf[BUF_SIZE] = {0,};
	unsigned long tot_file_size, read_file_size;

	srand(time(NULL)); // set seed for random function

	/*** First phase ***/
	TIMER_START();
	fd = open(filepath, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
	if(fd < 0){
		perror("Failed to open file\n");
		exit(1);
	}

	//TODO: this assume that LARGE_FILE_SIZE is bigger than block_size
	//      and integer multiply of block_size is LARGE_FILE_SIZE
	while(tot_file_size < LARGE_FILE_SIZE){
		//TODO: check size of written bytes
		wrt_size = write(fd, buf, block_size);
		tot_file_size += wrt_size;
	}
	TIMER_END();

	printf("Create(Open)/Write.\t %ld sec, %ld usec\n", TIMER_ELAPSE_SEC(), TIMER_ELAPSE_USEC());

	/*** Second phase ***/
	TIMER_START();
	read_file_size = 0;
	while(tot_file_size < read_file_size){
		rd_size = read(fd, buf, block_size);
		read_file_size += rd_size;
	}
	TIMER_END();

	printf("Seqeuntial Read.\t %ld sec, %ld usec\n", TIMER_ELAPSE_SEC(), TIMER_ELAPSE_USEC());

	/*** Third phase ***/

	//TODO: random access here.

	/*** Fourth phase ***/
	TIMER_START();
	unlink(fd);

	TIMER_END();

	printf("Unlink file.\t %ld sec, %ld usec\n", TIMER_ELAPSE_SEC(), TIMER_ELAPSE_USEC());
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
 * 3. lseek() random access, read 1 block
 * 4. lseek() random access, rewrite 1 block
 * 5. delete file
 *
 */

unsigned long long ulrand(){
	unsigned long long ret = 0;

	for (int i = 0; i < 5; ++i) {
		ret = (ret << 15) | (rand() & 0x7FFF);
	}

	return ret & 0xFFFFFFFFFFFFFFFFULL;
}

void fsmb_largefile_benchmark(const char * filepath, int block_size, int count){
	int fd, wrt_size, rd_size, ret;
	char buf[BUF_SIZE] = {0,};
    char filepath_r[1024] = "";
	unsigned long long tot_file_size, read_file_size, tmp;
	long int sec, usec;
	unsigned long long *rand_seq;
	int block_count = LARGE_FILE_SIZE/block_size;

	rand_seq = (unsigned long long*)malloc(block_count * sizeof(unsigned long long));

    printf("micro benchmarking for one big file RW\n");
    printf("Opn/Seq Wrt | Seq Read | Ran Read | Ran Wrt | Del\n");

	for (int iter = 0; iter < count; iter++) {
		/*** Rand value set ***/
		srand(time(NULL)); // set seed for random function

		// init
		for(unsigned long i=0;i<block_count;i++){
			rand_seq[i] = i;
		}

		// shuffle
		for (unsigned long i=block_count - 1; i > 0; i--) {
			unsigned long j = ulrand() % (i + 1);

			tmp = rand_seq[i];
			rand_seq[i] = rand_seq[j];
			rand_seq[j] = tmp;
		}


		/*** First phase ***/
		sprintf(filepath_r, "%s%d", filepath, iter);
		tot_file_size = 0;

		TIMER_START();
		fd = open(filepath_r, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (fd < 0) {
			perror("Failed to open file\n");
			free(rand_seq);
			exit(1);
		}

		//TODO: this assume that LARGE_FILE_SIZE is bigger than block_size
		//      and integer multiply of block_size is LARGE_FILE_SIZE
		while (tot_file_size < LARGE_FILE_SIZE) {
			//TODO: check size of written bytes
			wrt_size = write(fd, buf, block_size);
			if(wrt_size != block_size){
				printf("failed to write a whole block\n");
				exit(1);
			}
			tot_file_size += wrt_size;
		}
		TIMER_END();

		usec = TIMER_ELAPSE_USEC();
		sec = TIMER_ELAPSE_SEC();

		if (usec < 0) {
			usec = 1000000 + usec;
			sec--;
		}

		printf("%ld.%06ld \t", sec, usec);

		/*** Second phase ***/
		TIMER_START();
		read_file_size = 0;
		while (tot_file_size > read_file_size) {
			rd_size = read(fd, buf, block_size);
			if(rd_size != block_size){
				printf("failed to read a whole block\n");
				exit(1);
			}
			read_file_size += rd_size;
		}
		TIMER_END();

		usec = TIMER_ELAPSE_USEC();
		sec = TIMER_ELAPSE_SEC();

		if (usec < 0) {
			usec = 1000000 + usec;
			sec--;
		}

		printf("%ld.%06ld \t", sec, usec);

		/*** Third phase ***/
		TIMER_START();

		for (unsigned long long i = 0; i < block_count; i++) {
			ret = lseek(fd, rand_seq[i]*block_size, SEEK_SET);
			if(ret<0){
				perror("failed to lseek");
				exit(1);
			}
			rd_size = read(fd, buf, block_size);
			if(rd_size != block_size){
				printf("failed to read a whole block\n");
				exit(1);
			}
		}

		TIMER_END();

		usec = TIMER_ELAPSE_USEC();
		sec = TIMER_ELAPSE_SEC();

		if (usec < 0) {
			usec = 1000000 + usec;
			sec--;
		}

		printf("%ld.%06ld \t", sec, usec);

		/*** Fourth phase ***/
		TIMER_START();

		for (unsigned long long i = 0; i < block_count; i++) {
			ret = lseek(fd, rand_seq[i]*block_size, SEEK_SET);
			if(ret<0){
				perror("failed to lseek");
				exit(1);
			}
			wrt_size = write(fd, buf, block_size);
			if(wrt_size != block_size){
				printf("failed to write a whole block\n");
				exit(1);
			}
		}

		TIMER_END();

		usec = TIMER_ELAPSE_USEC();
		sec = TIMER_ELAPSE_SEC();

		if (usec < 0) {
			usec = 1000000 + usec;
			sec--;
		}

		printf("%ld.%06ld \t", sec, usec);

		/*** Fifth phase ***/
		TIMER_START();
		unlink(filepath_r);

		TIMER_END();

		usec = TIMER_ELAPSE_USEC();
		sec = TIMER_ELAPSE_SEC();

		if (usec < 0) {
			usec = 1000000 + usec;
			sec--;
		}

		printf("%ld.%06ld \n", sec, usec);
	}

	free(rand_seq);
}

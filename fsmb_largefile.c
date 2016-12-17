#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "fsmb_largefile.h"
#include "time_util.h"

#define BUF_SIZE 1024*1024 // 1MB, because the maximum size of block unit is 1MB
#define LARGE_FILE_SIZE 1024*1024*1024 // 1GB, TODO: TBD
#define STR_BUF_SIZE 1024

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

void fsmb_largefile_benchmark(const char * filepath, int block_size, int count, const char * log_filename){
	int fd, wrt_size, rd_size, ret, str_len;
	char buf[BUF_SIZE] = {0,};
	char str_buf[STR_BUF_SIZE];
	char filepath_r[1024] = "";
	unsigned long long tot_file_size, read_file_size, tmp;
	unsigned long long *rand_seq;
	int block_count = LARGE_FILE_SIZE/block_size;

	long int* elapse_time_ary[5][2]; // To save sec dif and nsec dif

	for (int i=0;i<5;i++){
		elapse_time_ary[i][0] = (long int*)malloc(sizeof(long int)*count);
		elapse_time_ary[i][1] = (long int*)malloc(sizeof(long int)*count);	
	}

	rand_seq = (unsigned long long*)malloc(block_count * sizeof(unsigned long long));
	for(int i=0;i<BUF_SIZE;i++){
		buf[i] = i % 2;
	}

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
				perror("failed to write a whole block\n");
				free(rand_seq);
				exit(1);
			}
			tot_file_size += wrt_size;
		}

		fsync(fd);

		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		elapse_time_ary[0][0][iter] = sec;
		elapse_time_ary[0][1][iter] = nsec;

		/*** Second phase ***/
		//rewind file pointer
		ret = lseek(fd, 0, SEEK_SET);
		if(ret<0){
			perror("failed to lseek");
			free(rand_seq);
			exit(1);
		}

		TIMER_START();
		read_file_size = 0;
		while (tot_file_size > read_file_size) {
			rd_size = read(fd, buf, block_size);
			if(rd_size != block_size){
				perror("failed to read a whole block\n");
				free(rand_seq);
				exit(1);
			}
			read_file_size += rd_size;
		}
		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		elapse_time_ary[1][0][iter] = sec;
		elapse_time_ary[1][1][iter] = nsec;

		/*** Third phase ***/
		TIMER_START();

		for (unsigned long long i = 0; i < block_count; i++) {
			ret = lseek(fd, rand_seq[i]*block_size, SEEK_SET);
			if(ret<0){
				perror("failed to lseek");
				free(rand_seq);
				exit(1);
			}
			rd_size = read(fd, buf, block_size);
			if(rd_size != block_size){
				perror("failed to read a whole block\n");
				free(rand_seq);
				exit(1);
			}
		}

		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		elapse_time_ary[2][0][iter] = sec;
		elapse_time_ary[2][1][iter] = nsec;

		/*** Fourth phase ***/
		TIMER_START();

		for (unsigned long long i = 0; i < block_count; i++) {
			ret = lseek(fd, rand_seq[i]*block_size, SEEK_SET);
			if(ret<0){
				perror("failed to lseek");
				free(rand_seq);
				exit(1);
			}
			wrt_size = write(fd, buf, block_size);
			if(wrt_size != block_size){
				perror("failed to write a whole block\n");
				free(rand_seq);
				exit(1);
			}
		}

		fsync(fd);

		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		elapse_time_ary[3][0][iter] = sec;
		elapse_time_ary[3][1][iter] = nsec;

		/*** Fifth phase ***/
		TIMER_START();
		unlink(filepath_r);

		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		elapse_time_ary[4][0][iter] = sec;
		elapse_time_ary[4][1][iter] = nsec;
	}

	fd = open(log_filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		perror("Failed to make a log file\n");
		exit(1);
	}

	str_len = sprintf(str_buf, "Micro benchmarking for one big file RW (%d bytes blk)\nOpn/Seq Wrt | Seq Read | Ran Read | Ran Wrt | Del\n", block_size);
	ret = write(fd, str_buf, str_len);
	if(ret != str_len){
		perror("Failed to write log file\n");
		exit(1);
	}
	
	for(int i=0;i<count;i++){
		str_len = sprintf(str_buf, "%ld.%09ld\t%ld.%09ld\t%ld.%09ld\t%ld.%09ld\t%ld.%09ld\n", 
			elapse_time_ary[0][0][i], elapse_time_ary[0][1][i],
			elapse_time_ary[1][0][i], elapse_time_ary[1][1][i],
			elapse_time_ary[2][0][i], elapse_time_ary[2][1][i],
			elapse_time_ary[3][0][i], elapse_time_ary[3][1][i],
			elapse_time_ary[4][0][i], elapse_time_ary[4][1][i]
			);
		ret = write(fd, str_buf, str_len);
		if(ret != str_len){
			perror("Failed to write log file\n");
			exit(1);
		}
	}

	close(fd);

	free(rand_seq);

	for (int i=0;i<5;i++){
		free(elapse_time_ary[i][0]);
		free(elapse_time_ary[i][1]);
	}

}

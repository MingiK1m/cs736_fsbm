#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "fsmb_smallfile.h"
#include "time_util.h"

#define BUF_SIZE 1024*1024 // 1mb
#define FILE_COUNT 1000 // 1k iteration
#define STR_BUF_SIZE 1024

/*
 * Design
 * 1. Create/open 10,000 files, write 1 block and close
 * 2. Open and read files.
 * 3. Delete files.
 *
 */

void fsmb_smallfile_benchmark(const char * filepath, long block_size, int count, const char * log_filename){
	int fd, ret_size, str_len;
	char buf[BUF_SIZE] = {0,};
	char str_buf[STR_BUF_SIZE];
	char filepath_r[1024] = "";

	for(int i=0;i<BUF_SIZE;i++){
		buf[i] = i % 2;
	}

	long int* elapse_time_ary[3][2]; // To save sec dif and nsec dif

	for (int i=0;i<3;i++){
		elapse_time_ary[i][0] = (long int*)malloc(sizeof(long int)*count);
		elapse_time_ary[i][1] = (long int*)malloc(sizeof(long int)*count);	
	}

	for (int cnt = 0; cnt < count; cnt++) {
		/*** First phase ***/
		TIMER_START();
		for (int i = 0; i < FILE_COUNT; i++) {
			//TODO: check sprintf
			sprintf(filepath_r, "%s%d_%d", filepath, cnt, i);

			fd = open(filepath_r, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			if (fd < 0) {
				perror("Failed to open file");
				exit(1);
			}

			ret_size = write(fd, buf, block_size);
			if(ret_size != block_size){
				perror("failed to write a whole block");
				exit(1);
			}
			
			fsync(fd);

			close(fd);
		}
		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		elapse_time_ary[0][0][cnt] = sec;
		elapse_time_ary[0][0][cnt] = nsec;

		/*** Second phase ***/
		TIMER_START();
		for (int i = 0; i < FILE_COUNT; i++) {
			sprintf(filepath_r, "%s%d_%d", filepath, cnt, i);

			fd = open(filepath_r, O_RDONLY, 0);
			if (fd < 0) {
				perror("Failed to open file");
				exit(1);
			}

			ret_size = read(fd, buf, block_size);
			if (ret_size != block_size) {
				perror("failed to read a whole block");
				exit(1);
			}

			close(fd);
		}
		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		elapse_time_ary[1][0][cnt] = sec;
		elapse_time_ary[1][0][cnt] = nsec;

		/*** Third phase ***/
		TIMER_START();
		for (int i = 0; i < FILE_COUNT; i++) {
			sprintf(filepath_r, "%s%d_%d", filepath, cnt, i);

			unlink(filepath_r);
		}
		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		elapse_time_ary[2][0][cnt] = sec;
		elapse_time_ary[2][0][cnt] = nsec;
	}

	fd = open(log_filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		perror("Failed to make a log file\n");
		exit(1);
	}

	str_len = sprintf(str_buf, "Micro benchmarking for lots of small files RW (%ld bytes blk)\nOpn/Wrt/Cls | Opn/Rd/Cls | Del\n", block_size);
	ret_size = write(fd, str_buf, str_len);
	if(ret_size != str_len){
		perror("Failed to write log file\n");
		exit(1);
	}
	
	for(int i=0;i<count;i++){
		str_len = sprintf(str_buf, "%ld.%09ld\t%ld.%09ld\t%ld.%09ld\n", 
			elapse_time_ary[0][0][i], elapse_time_ary[0][1][i],
			elapse_time_ary[1][0][i], elapse_time_ary[1][1][i],
			elapse_time_ary[2][0][i], elapse_time_ary[2][1][i]
			);
		ret_size = write(fd, str_buf, str_len);
		if(ret_size != str_len){
			perror("Failed to write log file\n");
			exit(1);
		}
	}

	close(fd);

	for (int i=0;i<3;i++){
		free(elapse_time_ary[i][0]);
		free(elapse_time_ary[i][1]);	
	}
}

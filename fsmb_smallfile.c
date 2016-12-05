#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "fsmb_smallfile.h"
#include "time_util.h"

/*
 * Design
 * 1. Create/open 10,000 files, write 1 block and close
 * 2. Open and read files.
 * 3. Delete files.
 *
 */

void fsmb_smallfile_benchmark(const char * filepath, long block_size, int count){
    int fd, ret_size;
    char buf[BUF_SIZE] = {0,};
    char filepath_r[1024] = "";
    long int sec, usec;

    printf("micro benchmarking for lots of small files RW\n");

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
				printf("failed to write a whole block");
				exit(1);
			}

			close(fd);
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
		for (int i = 0; i < FILE_COUNT; i++) {
			sprintf(filepath_r, "%s%d", filepath, i);

			fd = open(filepath_r, O_RDONLY, 0);
			if (fd < 0) {
				perror("Failed to open file");
				exit(1);
			}

			ret_size = read(fd, buf, block_size);
			if (ret_size != block_size) {
				printf("failed to read a whole block");
				exit(1);
			}

			close(fd);
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
		for (int i = 0; i < FILE_COUNT; i++) {
			sprintf(filepath_r, "%s%d", filepath, i);

			unlink(filepath_r);
		}
		TIMER_END();

		usec = TIMER_ELAPSE_USEC();
		sec = TIMER_ELAPSE_SEC();

		if (usec < 0) {
			usec = 1000000 + usec;
			sec--;
		}

		printf("%ld.%06ld\n", sec, usec);
	}
}

#include <stdio.h>
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
    int fd, wrt_size;
    char buf[BUF_SIZE] = {0,};
    char filepath_r[1024] = "";

    /*** First phase ***/
    TIMER_START();
    for(int i=0; i< FILE_COUNT; i++){
	//TODO: check sprintf
	sprintf(filepath_r, "%s%d", filepath, i);

	fd = open(filepath, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
	if(fd < 0){
	    perror("Failed to open file\n");
	    exit(1);
	}

	wrt_size = write(fd, buf, block_size);

	close(fd);
    }
    TIMER_END();

    printf("%ld sec, %ld usec\n", TIMER_ELAPSE_SEC(), TIMER_ELAPSE_USEC());

    /*** Second phase ***/

    /*** Third phase ***/
    TIMER_START();
    //TODO: del
    TIMER_END();

    printf("%ld sec, %ld usec\n", TIMER_ELAPSE_SEC(), TIMER_ELAPSE_USEC());
}

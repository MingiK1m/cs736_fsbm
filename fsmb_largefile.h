#ifndef __FSMB_LARGEFILE_H__
#define __FSMB_LARGEFILE_H__

#define BUF_SIZE 1024*1024 // 1MB, because the maximum size of block unit is 1MB
#define LARGE_FILE_SIZE 1024*1024*1024 // 4GB, TODO: TBD

void fsmb_largefile_benchmark(const char * filepath, int block_size, int count);

#endif /* __FSMB_LARGEFILE_H__ */

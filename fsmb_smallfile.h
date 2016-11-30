#ifndef __FSMB_SMALLFILE_H__
#define __FSMB_SMALLFILE_H__

#define BUF_SIZE 1024*1024 // 1mb

void fsmb_smallfile_benchmark(const char * filepath, long block_size, int count);

#endif /* __FSMB_SMALLFILE_H__ */

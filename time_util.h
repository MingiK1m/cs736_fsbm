#ifndef __TIME_UTIL_H__
#define __TIME_UTIL_H__

#include <sys/time.h>

#define TIMER_START() gettimeofday(&tv_start, 0)
#define TIMER_END() gettimeofday(&tv_end, 0)
#define TIMER_ELAPSE_SEC() (tv_end.tv_sec - tv_start.tv_sec)
#define TIMER_ELAPSE_USEC() (tv_end.tv_usec - tv_start.tv_usec)

struct timeval tv_start;
struct timeval tv_end;


#endif /* __TIME_UTIL_H__ */

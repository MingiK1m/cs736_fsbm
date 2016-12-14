#ifndef __TIME_UTIL_H__
#define __TIME_UTIL_H__

#include <time.h>
#include <assert.h>

#define TIMER_START() assert(clock_gettime( CLOCK_REALTIME, &ts_start)==0)
#define TIMER_END() assert(clock_gettime( CLOCK_REALTIME, &ts_end)==0)
#define TIMER_ELAPSE_SEC() (ts_end.tv_sec - ts_start.tv_sec)
#define TIMER_ELAPSE_NSEC() (ts_end.tv_nsec - ts_start.tv_nsec)

struct timespec ts_start;
struct timespec ts_end;

long int sec, nsec;

#endif /* __TIME_UTIL_H__ */

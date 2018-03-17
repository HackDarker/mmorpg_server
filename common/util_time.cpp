#include "util_time.h"

volatile uint32_t UNIXTIME;

uint32_t getMSTime()
{	
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

void updateFrameTime()
{	
	UNIXTIME = (uint32_t)time(NULL);
}

uint32_t getFrameTime()
{
	return UNIXTIME;
}

void Sleep(uint32_t ms)
{
	struct timespec t;
	t.tv_sec  = ms / 1000;
	t.tv_nsec = (ms % 1000) / 1000000;
	nanosleep(&t,NULL);
}

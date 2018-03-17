#include "thread.h"
#include "threadpool.h"
#include "condition.h"
#include "mutex.h"
#include "common.h"

bool g_running = true;

int main()
{
	ThreadPool::instance()->startUp();
	int i = 0;
	while(g_running)
	{
		i = i + 10;
		printf("now is %d\n",i);
		sleep(10);
	}
	return 0;
}

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <set>
#include <stdint.h>
#include "mutex.h"
#include "thread.h"
#include "common.h"

class ThreadPool
{
public:
	static ThreadPool* Instance()
	{
		static ThreadPool _instance;
		return &_instance;
	}

	void Startup();
	void Shutdown();
	void pushToFreeQueue(Thread *);
	Thread * startTthread(ThreadBase * executionTarget);
	void ExecuteTask(ThreadBase * executionTarget);
	uint32 getActiveThreadCount() { return (uint32)m_activeThreads.size(); }
	uint32 getFreeThreadCount() { return (uint32)m_freeThreads.size(); }
private:
	ThreadPool();
	
    std::set<Thread*> m_activeThreads;
	std::set<Thread*> m_freeThreads;
	Mutex m_mutex;
};

#endif

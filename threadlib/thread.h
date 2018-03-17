
#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include "mutex.h"
#include "common.h"

class ThreadBase
{
public:
	ThreadBase() {}
	virtual ~ThreadBase() {}
	
	virtual bool run() = 0;
	virtual void onShutdown() {}

	void setAffinity(int nCpu);

	pthread_t m_threadHandle;
};

class Thread
 {
 public:
 	Thread();
 	~Thread();
 	
	void setUp(pthread_t h);
	void suspend();
	void resume();	
	void join();
	uint32 getId();
	pthread_t getHandle();

	ThreadBase * executionTarget;
	Mutex setupMutex;
 private:
 	pthread_cond_t m_cond;
	pthread_mutex_t m_mutex;
	int m_threadId;
	pthread_t m_handle;
 }; 


#endif
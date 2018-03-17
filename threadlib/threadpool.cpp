#include "../common/util_time.h"
#include "threadpool.h"
#include <stdio.h>
#include <unistd.h>

#define THREAD_REVERSE 5

ThreadPool::ThreadPool()
{

}

void ThreadPool::Startup()
{
	for(int i=0; i < THREAD_REVERSE; i++)
	{
		startTthread(NULL);
	}

	Sleep(3000);
	printf("startUp, launched %u threads.\n",THREAD_REVERSE);
}

void ThreadPool::pushToFreeQueue(Thread *t)
{
	m_activeThreads.erase(t);
	m_freeThreads.insert(t);
	printf("Thread %u entered the free pool.\n", t->getId());
}

static void * thread_proc(void * param)
{
	Thread * t = (Thread*)param;
	printf("ThreadPool %u started.", t->getId());

	while(true)
	{
		if(t->executionTarget != NULL){
			if(t->executionTarget->run()){
				delete t->executionTarget;
			}
			t->executionTarget = NULL;
		}

		ThreadPool::Instance()->pushToFreeQueue(t);//没任务的时候线程放进空闲队列
		t->suspend();//等待唤醒
	}

	pthread_exit(0);
}

Thread * ThreadPool::startTthread(ThreadBase * executionTarget)
{
	Thread * t = new Thread;
	t->executionTarget = executionTarget;

	m_mutex.Lock();

	pthread_t handle;
	pthread_create(&handle, NULL, &thread_proc, (void*)t);
	if (!handle){
		printf("pthread_create fail !!!");
	}
	if(executionTarget) executionTarget->m_threadHandle = handle;
	t->setUp(handle);	

	m_mutex.UnLock();

	return t;
}

void ThreadPool::ExecuteTask(ThreadBase * executionTarget)
{
	printf("22222222222222222222222222222\n");
	Thread * t;
	m_mutex.Lock();
	
	if(!m_freeThreads.empty()){
		t = *m_freeThreads.begin();
		m_freeThreads.erase(m_freeThreads.begin());

		t->executionTarget = executionTarget;
		executionTarget->m_threadHandle = t->getHandle();
		t->resume();
		printf("Thread %u left the thread pool.", t->getId());
	}else{
		printf("has no free thread start a new thread\n");
		t = startTthread(executionTarget);
	}

	printf("Thread %u is now executing task at %p.", t->getId(), executionTarget);
	m_activeThreads.insert(t);

	m_mutex.UnLock();
}


void ThreadPool::Shutdown()
{
	m_mutex.Lock();

	uint32_t tcount = m_activeThreads.size() + m_freeThreads.size();
	printf("ThreadPool Shutting down %u threads.", tcount);

	for(std::set<Thread*>::iterator itr = m_freeThreads.begin();itr != m_freeThreads.end();++itr){
		(*itr)->executionTarget = NULL; 
		(*itr)->resume();
	}

	for(std::set<Thread*>::iterator itr = m_activeThreads.begin(); itr != m_activeThreads.end(); ++itr){
		if((*itr)->executionTarget){
			(*itr)->executionTarget->onShutdown();
		}
	}

	m_mutex.UnLock();
}

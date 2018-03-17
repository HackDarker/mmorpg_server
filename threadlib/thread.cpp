
#include "thread.h"

volatile int threadid_count = 0;
int GenerateThreadId()
{
	int i = ++threadid_count;
	return i;
}

Thread::Thread()
{

}

Thread::~Thread()
{
	pthread_mutex_destroy(&m_mutex);
	pthread_cond_destroy(&m_cond);
}

void Thread::setUp(pthread_t h)
{
	m_handle = h;
	pthread_mutex_init(&m_mutex,NULL);
	pthread_cond_init(&m_cond,NULL);
	m_threadId = GenerateThreadId();
}

void Thread::suspend()
{
	pthread_cond_wait(&m_cond, &m_mutex);
}

void Thread::resume()
{
	pthread_cond_signal(&m_cond);
}

void Thread::join()
{
	pthread_join(m_handle,NULL);
}

uint32 Thread::getId()
{
	return (uint32_t)m_threadId;
}

pthread_t Thread::getHandle()
{
	return m_handle;
}

#include "condition.h"
#include "mutex.h"

Condition::Condition(Mutex &lock):m_lock(lock)
{
	pthread_cond_init(&m_cond, NULL);
}

Condition::~Condition()
{
	pthread_cond_destroy(&m_cond);
}

void Condition::wait()
{
	pthread_cond_wait(&m_cond, &m_lock.m_mutex);
}

bool Condition::timeoutWait(time_t seconds)
{
	timespec tv;
	tv.tv_nsec = 0;
	tv.tv_sec = seconds;
	if(pthread_cond_timedwait(&m_cond, &m_lock.m_mutex, &tv) == 0)
		return true;
	else
		return false;
}

void Condition::notify()
{
	pthread_cond_signal(&m_cond);
}

void Condition::notifyAll()
{
	pthread_cond_broadcast(&m_cond);
}



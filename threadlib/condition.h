
#ifndef _CONDITION_H_
#define _CONDITION_H_

#include "mutex.h"

class Condition
{
	public:
		Condition(Mutex &lock);
		~Condition();

		void wait();
		bool timeoutWait(time_t seconds);
		void notify();
		void notifyAll();
	private:
		pthread_cond_t m_cond;
		Mutex &m_lock;
};

#endif



#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <pthread.h>

class Condition;

class Mutex 
{
	friend class Condition;
	
	public:
		Mutex();
		~Mutex();

		void Lock();
		bool TryLock();
		void UnLock();
	private:
		pthread_mutex_t m_mutex;
};

#endif



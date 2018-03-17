#ifndef FASTQUEUE_H
#define FASTQUEUE_H

template<class T, class LOCK>
class FastQueue
{
	struct node
	{
		T element;
		node * next;
	};

	node * last;
	node * first;
	LOCK m_lock;

public:

	FastQueue()
	{
		last = 0;
		first = 0;
	}

	~FastQueue()
	{
		Clear();
	}

	void Clear()
	{
		while(last != 0)
			Pop();
	}

	void Push(T elem)
	{
		m_lock.Lock();
		node * n = new node;
		if(last)
			last->next = n;
		else
			first = n;

		last = n;
		n->next = 0;
		n->element = elem;
		m_lock.UnLock();
	}

	T Pop()
	{
		m_lock.Lock();
		if(first == 0)
		{
			m_lock.UnLock();
			return reinterpret_cast<T>(0);
		}
        
		T ret = first->element;
		node * td = first;
		first = td->next;
		if(!first)
			last = 0;

		delete td;
		m_lock.UnLock();
		return ret;
	}

	T front()
	{
		m_lock.Lock();
		if(first == 0)
		{
			m_lock.UnLock();
			return reinterpret_cast<T>(0);
		}

		T ret = first->element;
		m_lock.UnLock();
		return ret;
	}

	void pop_front()
	{
		m_lock.Lock();
		if(first == 0)
		{
			m_lock.UnLock();
			return;
		}

		node * td = first;
		first = td->next;
		if(!first)
			last = 0;

		delete td;
		m_lock.UnLock();
	}

	bool HasItems()
	{
		bool ret;
		m_lock.Lock();
		ret = (first != 0);
		m_lock.UnLock();
		return ret;
	}
};

#endif
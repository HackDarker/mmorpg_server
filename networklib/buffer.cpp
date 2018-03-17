#include "buffer.h"

Buffer::Buffer():m_ptr(0),m_cur(0),m_cap(0)
{

}

Buffer::~Buffer()
{

}

int Buffer::BuffExpand(uint32 need)
{
	uint32_t cap = ceil((m_cap + need)/1024.0) * 1024;
	if (cap < m_cap) return -1;
	char* ptr = (char *)realloc(m_ptr,cap);
	if (!ptr) return -2;
	m_ptr = ptr;
	m_cap = cap;
	return 0;
}

bool Buffer::PushData(const char * bytes,uint32 size)
{
	if (!bytes || size <= 0) return false;
	uint32_t freeSize = GetFreeSize();
	if (freeSize < size)
	{
		if (BuffExpand(size - freeSize)) return false;
	}
	memcpy(m_ptr,bytes,	size);
	BufferWrited(size);
	return true;
}

bool Buffer::ReadData(uint8 * destination,uint32 size)
{
	if (size > GetCur()) return false;

	memcpy(destination,m_ptr,size);
	BufferReaded(size);
	return true;
}

int Buffer::BufferReaded(uint32 n)
{
	if (n > m_cur) return -1;
	uint32_t left = m_cur - n;
	memcpy(m_ptr,m_ptr+n,left);
	m_cur -= n;
	return 0;
}

int Buffer::BufferWrited(uint32 n)
{
	uint32_t fz = GetFreeSize();
	if (fz < n) return -1;
	m_cur += n;
	return 0;
}

uint32 Buffer::GetFreeSize()
{
	return m_cap - m_cur;
}

char* Buffer::GetCurPtr()
{
	return m_ptr + m_cur;
}

uint32 Buffer::GetCur()
{
	return m_cur;
}

char* Buffer::GetStart()
{
	return m_ptr;
}

void Buffer::Reset()
{
	if (m_ptr) free(m_ptr);
	m_ptr = 0;
	m_cur = 0;
	m_cap = 0;
}




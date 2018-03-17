#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "../common/common.h"

class Buffer
{
public:
	Buffer();
	~Buffer();

	int BufferReaded(uint32_t n);
	bool PushData(const char * bytes,uint32_t size);
	bool ReadData(uint8_t * destination,uint32_t size);
	int BuffExpand(uint32_t need);
	int BufferWrited(uint32_t n);
	uint32_t GetFreeSize();
	uint32_t GetCur();
	char* GetCurPtr();
	char* GetStart();
	void Reset();

private:
	char *m_ptr;
	uint32 m_cur;
	uint32 m_cap;
};

#endif
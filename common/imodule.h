#ifndef IMODULE_H
#define IMODULE_H

#include "common.h"

class IModule 
{
public:
	virtual int Init() = 0;
	virtual int Start() = 0;
	virtual int Update(uint32_t loopCount) = 0;
	virtual int Stop() = 0;
};

#endif

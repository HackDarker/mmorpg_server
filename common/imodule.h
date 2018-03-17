#ifndef IMODULE_H
#define IMODULE_H

class IModule 
{
public:
	virtual int Init() = 0;
	virtual int Start() = 0;
	virtual int Update() = 0;
	virtual int Stop() = 0;
};

#endif
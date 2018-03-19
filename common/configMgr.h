#ifndef CONFIGMGR_H
#define CONFIGMGR_H

#include <map>

extern "C" 
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
};

class ConfigMgr
{
public:
	~ConfigMgr();
	static ConfigMgr* Instance()
	{
		static ConfigMgr inst;
		return &inst;
	}

	bool LoadConfig(const char* filename);
	bool GetBool(const char* key,int opt);
	int  GetInt(const char* key,int opt);
	const char* GetString(const char* key,const char* opt); 
private:
	ConfigMgr();
	std::map<const char *key,char* value> m_confMap;
};

#endif
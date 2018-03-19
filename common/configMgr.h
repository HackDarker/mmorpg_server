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
	bool GetBool(std::string key,int opt);
	int  GetInt(std::string keyy,int opt);
	char* GetString(std::string key,char* opt); 
private:
	ConfigMgr();
	std::map<std::string key,std::string value> m_confMap;
};

#endif
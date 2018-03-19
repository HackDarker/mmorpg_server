#ifndef CONFIGMGR_H
#define CONFIGMGR_H

#include <string>
#include <map>

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
	int  GetInt(std::string key,int opt);
	char* GetString(std::string key,char* opt); 
private:
	ConfigMgr();

	std::map<std::string,std::string> m_confMap;
};

#endif
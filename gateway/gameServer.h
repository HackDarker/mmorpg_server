#ifndef GAMESERVER_H
#define GAMESERVER_H

#include "../common/util_time.h"
#include "../common/common.h"
#include <string>

class GameServer
{
public:
	~GameServer();
	static GameServer* Instance()
	{
		static GameServer inst;
		return &inst;
	}

	bool Run();
	void RegisterModule(const char* module_name, IModule* module);
private:
	GameServer();

	void Startup();
	void Loop();
	void Stop();
	
	std::string m_moduleName;
	IModule* m_module;
};

#endif
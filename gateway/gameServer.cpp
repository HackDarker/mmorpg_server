#include "gameServer.h"
#include "../common/imodule.h"
#include "../threadlib/threadpool.h"
#include "../networklib/socketMgr.h"

volatile bool g_running = true;

GameServer::GameServer()
{

}

GameServer::~GameServer()
{
	if (m_module) delete m_module;
}

void GameServer::RegisterModule(const char* module_name, IModule* module)
{
	m_moduleName = module_name;
	m_module 	 = module;
}

void GameServer::Startup()
{
	ThreadPool::Instance()->Startup();
	SocketMgr::Instance()->Startup();

	m_module->Init();
	m_module->Start();
}

void GameServer::Loop()
{
	uint32_t loop_count = 0;
	uint32_t start_time;
	uint32_t diff_time;

	while(g_running)
	{
		start_time = getMSTime();
		loop_count++;

		updateFrameTime();
		m_module->Update(loop_count);

		diff_time = getMSTime() - start_time;
		if(diff_time < 50){
			Sleep(50 - diff_time);
		}
	}
}

void GameServer::Stop()
{
	ThreadPool::Instance()->Shutdown();
	SocketMgr::Instance()->CloseAll();
	m_module->Stop();
}

bool GameServer::Run()
{	
	Startup();
	Loop();
	Stop();

	return true;
}


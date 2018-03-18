#include "../common/common.h"
#include "../common/util_time.h"
#include "../common/lua_tinker.h"
#include "../networklib/worldPacket.h"
#include "../threadlib/threadpool.h"
#include "luaEngine.h"
#include "luaProcMgr.h"

#define UPDATE_DELAY 40

LuaProcMgr::LuaProcMgr()
{
	m_runner = new LuaProcThread();
}

LuaProcMgr::~LuaProcMgr()
{
	if (m_runner) delete m_runner;
}

void LuaProcMgr::Startup()
{
    ThreadPool::Instance()->ExecuteTask(m_runner);
}

/*======================================LuaProcThread======================================*/
LuaProcThread::LuaProcThread()
{
	m_running = false;
	m_engine = new LuaEngine();
}

LuaProcThread::~LuaProcThread()
{
	if (m_engine) delete m_engine;
}

bool LuaProcThread::run()
{
	m_running = true;

	m_engine->Init();
	lua_tinker::call<void>(m_engine->L, "c_test");
	lua_tinker::call<void>(m_engine->L, "c_initDatabase");
	lua_tinker::call<void>(m_engine->L, "c_initWorld");

	MsgPacket* packet;
	uint32_t startTime;
	uint32_t diff;
	while(m_running) {
		startTime = getMSTime();
		while ((packet = m_queue.Pop())) {
			HandlePacket(*packet);
			delete packet;
		}

		diff = getMSTime() - startTime;
		if(diff < UPDATE_DELAY) {
			Sleep( UPDATE_DELAY - diff );
		}
	}

	return false;
}

void LuaProcThread::HandlePacket(WorldPacket& packet)
{	
	printf("111111111111111111111HandlePacket11111111111111\n");
	uint16_t opcode = packet.GetOpcode();
	uint16_t size   = packet.Size();
	WorldPacket data(opcode,size);
	data.ReadPacket(packet);


	m_engine->ProcQuery(33333, 44444, &data);
}

void LuaProcThread::onShutdown()
{
	m_running = false;
}
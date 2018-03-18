#ifndef LUAPROCMGR_H
#define LUAPROCMGR_H

#include "../common/fastQueue.h"
#include "../threadlib/thread.h"
#include "../threadlib/mutex.h"

class WorldPacket;
class LuaEngine;

class LuaProcThread : public ThreadBase
{
public:
	LuaProcThread();
	virtual ~LuaProcThread();

    virtual bool run();
	virtual void onShutdown();
	void AddPacket(WorldPacket* packet) { if(packet) m_queue.Push(packet); }
	void HandlePacket(WorldPacket& packet);
private:
	bool m_running;
	LuaEngine* m_engine;
	FastQueue<WorldPacket*, Mutex> m_queue;
};

class LuaProcMgr
{
public:
	virtual ~LuaProcMgr();
	static LuaProcMgr* Instance()
	{
		static LuaProcMgr inst;
		return &inst;
	}

	void Startup();
	void AddPacket(WorldPacket* packet){if(m_runner) m_runner->AddPacket(packet);}
private:
	LuaProcMgr();
	LuaProcThread* m_runner;
};

#endif
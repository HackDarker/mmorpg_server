#ifndef LUAPROCMGR_H
#define LUAPROCMGR_H

#include "../common/fastQueue.h"
#include "../threadlib/thread.h"
#include "../threadlib/mutex.h"

class MsgPacket;
class LuaEngine;

class LuaProcThread : public ThreadBase
{
public:
	LuaProcThread();
	virtual ~LuaProcThread();

    virtual bool run();
	virtual void onShutdown();
	void AddPacket(MsgPacket* packet) { if(packet) m_queue.Push(packet); }
	void HandlePacket(MsgPacket& packet);
private:
	bool m_running;
	LuaEngine* m_engine;
	FastQueue<MsgPacket*, Mutex> m_queue;
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
	void AddPacket(MsgPacket* packet){if(m_runner) m_runner->AddPacket(packet);}
private:
	LuaProcMgr();
	LuaProcThread* m_runner;
};

#endif
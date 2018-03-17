#ifndef DBMODULE_H
#define DBMODULE_H

#include "../common/common.h"
#include "../common/imodule.h"
#include <string>
#include <vector>

#define MAX_COMMSERVER_NUM 100

static const char DB_MODULE[] = "DbModule";

class ServerNetworkCallback;
class SocketMgr;
class MsgPacket;
class CommServer;

class DbModule:public IModule
{
	friend class ServerNetworkCallback;
public:
	DbModule();
	~DbModule();

	virtual int Init();
	virtual int Start();
	virtual int Update();
	virtual int Stop();

	void OnRecvCommserverMsg(NetID netid, MsgPacket* packet);
	void OnCommserverDisconnect(NetID netid);
private:
	bool ListenCommserver();
	void OnRegisterCommserver(const char *data);

	uint32_t	m_current_time;
	SocketMgr*	m_network;
	ServerNetworkCallback* m_network_callback;
	
	CommServer*	m_commserver_list[MAX_COMMSERVER_NUM];
};

#endif

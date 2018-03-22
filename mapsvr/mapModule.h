#ifndef MAPMODULE_H
#define MAPMODULE_H

#include "../common/common.h"
#include "../common/imodule.h"
#include <string>
#include <vector>

static const char MAP_MODULE[] = "MapModule";

class ServerInternalNetCallback;
class SocketMgr;
class WorldPacket;

class MapModule: public IModule
{
	friend class ServerInternalNetCallback;
public:
	MapModule();
	virtual ~MapModule();

	virtual int Init();
	virtual int Start();
	virtual int Update();
	virtual int Stop();

	void OnRecvCommserverMsg(NetID netid, const WorldPacket* packet);
	void OnCommserverDisconnect(NetID netid);
private:
	bool ConnectToGlobalServer();
	bool ConnectToDbServer();

	bool RegisterToGlobal();
	bool RegisterToDatabase();

	void OnRecvGlobalServerMsg(const WorldPacket* packet);
	void OnRecvDbServerMsg(const WorldPacket* packet);
	
	SocketMgr*	m_network;
	ServerInternalNetCallback* m_internal_network_callback;
	uint32_t		m_current_time;
	NetID			m_global_server_id;
	NetID                   m_db_server_id;
};

#endif

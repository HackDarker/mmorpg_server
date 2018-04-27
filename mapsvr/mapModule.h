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

struct CommClient
{
	uint16_t  type;
	NetID	 netId;
	uint32_t retryTime;
	uint16_t serverPort;
	std::string serverIp;
	
	CommClient()
	{
		type  = 0;
		netId = 0;
		retryTime = 0;
	}
};

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

	bool RegisterToGlobalServer();
	bool RegisterToDbServer();

	void OnRecvGlobalServerMsg(const WorldPacket* packet);
	void OnRecvDbServerMsg(const WorldPacket* packet);
	
	SocketMgr*	m_network;
	ServerInternalNetCallback* m_internal_network_callback;
	uint32_t		m_current_time;

	CommClient   m_gameClient;
	CommClient   m_databaseClient;
};

#endif

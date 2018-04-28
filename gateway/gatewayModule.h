#ifndef GATEWAYMODULE_H
#define GATEWAYMODULE_H

#include "../common/common.h"
#include "../common/imodule.h"
#include <string>
#include <vector>

static const char GATEWAY_MODULE[] = "GatewayModule";

class ServerNetworkCallback;
class ServerInternalNetCallback;
class SocketMgr;
class WorldPacket;

struct User
{
	User():netid(-1), last_active_time(0), has_checked(false), ip(""){}
	NetID	netid;
	uint32_t last_active_time;
	bool	 has_checked;
	std::string	ip;
	void Reset()
	{
		netid = -1;
		last_active_time = 0;
		has_checked = false;
		ip = "";
	}
};

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

class GatewayModule: public IModule
{
	friend class ServerNetworkCallback;
	friend class ServerInternalNetCallback;
public:
	GatewayModule();
	~GatewayModule();

	virtual int Init();
	virtual int Start();
	virtual int Update(uint32_t loopCount);
	virtual int Stop();

	void OnRecvUserMsg(NetID netid, const WorldPacket* packet);
	void OnUserDisconnect(NetID netid);
private:
	bool ListenForUser();
	bool ConnectToLoginServer();
	bool ConnectToGlobalServer();

	void RegisterToLogin();
	void RegisterToGlobalServer();
	void OnRecvLoginServerMsg(const WorldPacket* packet);

	SocketMgr*	m_network;
	ServerNetworkCallback* m_network_callback;
	ServerInternalNetCallback* m_internal_network_callback;

	uint32_t		m_current_time;
	
	CommClient  m_loginClient;
	CommClient  m_gameClient;
	User*       m_user_list;
	uint32_t    m_user_size;
	void		ResizeUserList(uint32_t size);
};

#endif

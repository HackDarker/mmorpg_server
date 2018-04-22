#ifndef LOGINMODULE_H
#define LOGINMODULE_H

#include "../common/common.h"
#include "../common/imodule.h"
#include <string>
#include <vector>

static const char LOGINWAY_MODULE[] = "LoginModule";

class ServerInternalNetCallback;
class SocketMgr;
class WorldPacket;

class LoginModule:public IModule
{
	friend class ServerInternalNetCallback;
public:
	LoginModule();
	~LoginModule();

	virtual int Init();
	virtual int Start();
	virtual int Update();
	virtual int Stop();

	void OnRecvGateWayMsg(NetID netid, const WorldPacket* packet);
	void OnGateWayDisconnect(NetID netid);
private:
	bool ListenForGateway();
	bool ConnectToDbServer();
	bool ConnectToGlobalServer();
	void OnRegisterGateway(const char *data);

	uint32_t	m_current_time;
	SocketMgr*	m_network;
	ServerInternalNetCallback* m_internal_network_callback;
	
	struct GateWay 
	{
		GateWay():netid(-1), index(-1), last_active_time(0), has_checked(false), ip(""){}
		NetID	 netid;
		int		 index;
		std::string	 ip;
		uint16_t port;
		bool	has_checked;
		uint32_t last_active_time;
		void Reset()
		{
			netid = -1;
			index = -1;
			ip    = "";
			port  = 0;
			last_active_time = 0;
			has_checked = false;
		}
	};
	GateWay*     m_gateway_list;
	unsigned int m_gateway_size;
	void		ResizeGateWayList(uint32_t size);
};

#endif

#ifndef LOGINMODULE_H
#define LOGINMODULE_H

#include "../common/common.h"
#include "../common/imodule.h"
#include <string>
#include <vector>

static const char LOGINWAY_MODULE[] = "LoginModule";

class ServerNetworkCallback;
class ServerInternalNetCallback;
class SocketMgr;
class MsgPacket;

class LoginModule:public IModule
{
	friend class ServerInternalNetCallback;
	friend class ServerNetworkCallback;
public:
	LoginModule();
	~LoginModule();

	virtual int Init();
	virtual int Start();
	virtual int Update();
	virtual int Stop();

	void OnRecvGateWayMsg(NetID netid, const MsgPacket* packet);
	void OnOtherServerDisconnect(NetID netid);
private:
	bool ListenForGateway();
	void OnRegisterGateway(const char *data);

	uint32_t	m_current_time;
	SocketMgr*	m_network;
	ServerNetworkCallback* m_network_callback;
	ServerInternalNetCallback* m_internal_network_callback;
	
	struct GateWay 
	{
		GateWay():netid(-1), index(-1), last_active_time(0), has_checked(false), ip(0){}
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
			ip    = 0;
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

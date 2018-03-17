#ifndef LOGINMODULE_H
#define LOGINMODULE_H

#include "../common/common.h"
#include "../common/imodule.h"
#include <string>
#include <vector>

static const char GLOBAL_MODULE[] = "GlobalModule";

class ServerInternalNetCallback;
class SocketMgr;
class WorldPacket;

class GlobalModule:public IModule
{
	friend class ServerInternalNetCallback;
public:
	GlobalModule();
	~GlobalModule();

	virtual int Init();
	virtual int Start();
	virtual int Update();
	virtual int Stop();

	void OnRecvCommserverMsg(NetID netid, const WorldPacket* packet);
	void OnCommserverDisconnect(NetID netid);
private:
	bool ListenForCommserver();
	void OnRegisterCommserver(const WorldPacket* packet);

	uint32_t	m_current_time;
	SocketMgr*	m_network;
	ServerInternalNetCallback* m_internal_network_callback;
	
	struct Commserver 
	{
		Commserver():netid(-1), server_type(-1), last_ping(0), has_checked(false){}
		NetID	  netid;
		uint32_t  server_type;
		uint32_t  last_ping;
		bool	  has_checked;
		void Reset()
		{
			netid        = -1;
			server_type  = -1;
			last_ping    = 0;
			has_checked  = false;
		}
	};

	Commserver*  m_commserver_list;
	uint32_t     m_commserver_size;
	void		 ResizeCommserverList(uint32_t size);
};

#endif

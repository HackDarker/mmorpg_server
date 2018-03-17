#include "../common/util_time.h"
#include "../common/imodule.h"
#include "globalModule.h"

class ServerInternalNetCallback : public IEngineNetCallback
{
public:
	ServerInternalNetCallback(GlobalModule *global):m_globalsvr(global),  m_cur_connect_num(0){}
	virtual ~ServerInternalNetCallback(){}
	virtual void OnAccept(NetID netid, std::string ip, uint16_t port)
	{
		m_cur_connect_num++;
		printf("Loginsvr OnAccept netid is ============%u\n", netid);

		if (netid >= m_globalsvr->m_commserver_size)
		{
			while (netid >= m_globalsvr->m_commserver_size)
			{
				m_globalsvr->ResizeCommserverList(m_globalsvr->m_commserver_size * 2);
			}
		}
		m_globalsvr->m_commserver_list[netid].netid = netid;
		m_globalsvr->m_commserver_list[netid].last_ping = m_globalsvr->m_current_time;
	}
	virtual void OnRecv(NetID netid, const WorldPacket* netPacket)
	{
		printf("Loginsvr OnRecv from netid ===========%u\n",netid);
		if (netid < m_globalsvr->m_commserver_size)
		{
			m_globalsvr->OnRecvCommserverMsg(netid, netPacket);
			m_globalsvr->m_commserver_list[netid].last_ping = m_globalsvr->m_current_time;
		}
	}
	virtual void OnDisconnect(NetID netid)
	{
		printf("OnDisconnect=================\n", netid);
		--m_cur_connect_num;
	}
private:
	GlobalModule *m_globalsvr;

	int m_cur_connect_num;
};

GlobalModule::GlobalModule()
{
	m_network = SocketMgr::instance();
	m_internal_network_callback = new ServerInternalNetCallback(this);
	m_current_time = 0;
}

GlobalModule::~GlobalModule()
{
	delete m_internal_network_callback;
	free(m_commserver_list);
}

int GlobalModule::Init()
{
	return true;
}

int GlobalModule::Start()
{
	m_network->RegisterCallback(SOCKET_TYPE_INTER,m_internal_network_callback);

	if (!ListenForCommserver())
	{
		return false;
	}

	return true;
}

int GlobalModule::Update()
{
	m_current_time = getFrameTime();
	printf("now time is %u\n", m_current_time);
}

int GlobalModule::Stop()
{
	return true;
}

bool GlobalModule::ListenForCommserver()
{
	std:string host_ip = "0.0.0.0";
	uint16_t listen_port = 8001;
	int ret = m_network->Listen(host_ip.c_str(),listen_port);
	if (ret < 0)
	{
		printf("NetListen return FAIL==ret==%d\n", ret);
		return false;
	}
	printf("ListenForCommserver begin=======on====port:%u\n", listen_port);
}

void GlobalModule::ResizeCommserverList(uint32_t size)
{
	if (m_commserver_size >= size) return;
	
	m_commserver_list = (Commserver *)realloc(m_commserver_list, size * sizeof(Commserver));
	for (uint32_t i = m_commserver_list; i < size; i++)
	{
		m_commserver_list[i].Reset();
	}
	m_commserver_size = size;
}

void GlobalModule::OnRecvCommserverMsg(NetID netid, const WorldPacket* packet)
{
	printf("OnRecvCommserverMsg=================from netid %u\n", netid);
}

void GlobalModule::OnCommserverDisconnect(NetID netid)
{
	printf("OnRecvCommserverMsg====netid is %u\n", netid);
}


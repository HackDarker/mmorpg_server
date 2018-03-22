#include "../common/util_time.h"
#include "../common/imodule.h"
#include "../networklib/socketMgr.h"
#include "loginModule.h"

class ServerNetworkCallback : public IEngineNetCallback
{
public:
	ServerNetworkCallback(LoginModule *loginsvr):m_loginsvr(loginsvr),  m_cur_connect_num(0){}
	virtual ~ServerNetworkCallback(){}
	virtual void OnAccept(NetID netid, std::string ip, uint16_t port)
	{
		m_cur_connect_num++;
		printf("Loginsvr OnAccept netid is ============%u\n", netid);

		if (netid >= m_loginsvr->m_gateway_size)
		{
			while (netid >= m_loginsvr->m_gateway_size)
			{
				m_loginsvr->ResizeGateWayList(m_loginsvr->m_gateway_size * 2);
			}
		}
		m_loginsvr->m_gateway_list[netid].netid = netid;
		m_loginsvr->m_gateway_list[netid].ip   = ip;
		m_loginsvr->m_gateway_list[netid].port = port;
		m_loginsvr->m_gateway_list[netid].last_active_time = m_loginsvr->m_current_time;
	}
	virtual void OnRecv(NetID netid,WorldPacket* netPacket)
	{
		printf("Loginsvr OnRecv from netid ===========%u\n",netid);
		if (netid < m_loginsvr->m_gateway_size)
		{
			m_loginsvr->OnRecvGateWayMsg(netid, netPacket);
			m_loginsvr->m_gateway_list[netid].last_active_time = m_loginsvr->m_current_time;
		}
	}
	virtual void OnDisconnect(NetID netid)
	{
		printf("OnDisconnect=================\n", netid);
		--m_cur_connect_num;
	}
private:
	LoginModule *m_loginsvr;

	int m_cur_connect_num;
};

class ServerInternalNetCallback : public IEngineNetCallback
{
public:
	ServerInternalNetCallback(LoginModule *loginsvr):m_loginsvr(loginsvr){}
	virtual ~ServerInternalNetCallback(){}
	virtual void OnAccept(NetID netid, std::string ip, uint16_t port)
	{
		printf("Internal Network OnAccept netid:%d\nIP:%s port:%u", netid,ip.c_str(),port);
	}
	virtual void OnRecv(NetID netid, WorldPacket* netPacket)
	{
		printf("ServerInternalNetCallback OnRecv======%s\n", netid);

	}
	virtual void OnDisconnect(NetID netid)
	{
		printf(" ServerInternalNetCallback OnDisconnect===========\n", netid);
	}
private:
	LoginModule *m_loginsvr;
};

LoginModule::LoginModule()
:m_network(0), m_network_callback(0), m_internal_network_callback(0),
m_current_time(0)
{
	m_network_callback = new ServerNetworkCallback(this);
	m_internal_network_callback = new ServerInternalNetCallback(this);
	m_network = SocketMgr::Instance();
}

LoginModule::~LoginModule()
{
	delete m_network_callback;
	delete m_internal_network_callback;
	free(m_gateway_list);
}

int LoginModule::Init()
{
	return true;
}

int LoginModule::Start()
{
	m_network->RegisterCallback(SOCKET_TYPE_CLIENT,m_network_callback);
	m_network->RegisterCallback(SOCKET_TYPE_INTER,m_internal_network_callback);

	if (!ListenForGateway())
	{
		return false;
	}

	return true;
}

int LoginModule::Update()
{
	m_current_time = getFrameTime();
	printf("now time is %u\n", m_current_time);
}

int LoginModule::Stop()
{
	return true;
}

bool LoginModule::ListenForGateway()
{
	std::string host_ip = "127.0.0.1";
	uint16_t listen_port = 8001;
	int ret = m_network->Listen(host_ip.c_str(),listen_port);
	if (ret < 0)
	{
		printf("NetListen return FAIL==ret==%d\n", ret);
		return false;
	}
	printf("ListenForGateWay begin=======on====port:%u\n", listen_port);
}

void LoginModule::ResizeGateWayList(uint32_t size)
{
	if (m_gateway_size >= size) return;
	
	m_gateway_list = (GateWay *)realloc(m_gateway_list, size * sizeof(GateWay));
	for (uint32_t i = m_gateway_size; i < size; i++)
	{
		m_gateway_list[i].Reset();
	}
	m_gateway_size = size;
}

void LoginModule::OnRecvGateWayMsg(NetID netid, const WorldPacket* packet)
{
	printf("OnRecvGateWayMsg=================from netid %u\n", netid);
}

void LoginModule::OnGateWayDisconnect(NetID netid)
{
	printf("OnGateWayDisconnect====netid is %u\n", netid);
}


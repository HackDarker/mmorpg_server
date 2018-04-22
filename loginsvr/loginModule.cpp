#include "../common/util_time.h"
#include "../common/imodule.h"
#include "../networklib/socketMgr.h"
#include "loginModule.h"

class ServerInternalNetCallback : public IEngineNetCallback
{
public:
	ServerInternalNetCallback(LoginModule *loginsvr):m_loginsvr(loginsvr),  m_cur_connect_num(0){}
	virtual ~ServerInternalNetCallback(){}
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

LoginModule::LoginModule()
{	
	m_current_time = 0;
	m_network = SocketMgr::Instance();
	m_internal_network_callback = new ServerInternalNetCallback(this);
}

LoginModule::~LoginModule()
{
	delete m_internal_network_callback;
	free(m_gateway_list);
}

int LoginModule::Init()
{
	return true;
}

int LoginModule::Start()
{
	m_network->RegisterCallback(SOCKET_TYPE_INTER,m_internal_network_callback);

	if (!ListenForGateway()){
		return false;
	}

	if (!ConnectToDbServer()){
		return false;
	}

	return true;
}

int LoginModule::Update()
{
	m_current_time = getFrameTime();
	//printf("now time is %u\n", m_current_time);
}

int LoginModule::Stop()
{
	return true;
}

bool LoginModule::ListenForGateway()
{
	std::string host_ip = "127.0.0.1";
	uint16_t listen_port = 8003;
	int ret = m_network->Listen(host_ip.c_str(),listen_port);
	if (ret < 0)
	{
		printf("NetListen return FAIL==ret==%d\n", ret);
		return false;
	}
	printf("ListenForGateWay begin=======on====port:%u\n", listen_port);

	return true;
}

bool LoginModule::ConnectToDbServer()
{
	std::string db_server_ip = "127.0.0.1";
	uint16_t db_server_port  = 8001;

	int ret = m_network->Connect(db_server_ip.c_str(), db_server_port);
	if (ret < 0)
	{
		printf("Connect to DBServer[%s:%d] Fail!==ret==%d", db_server_ip.c_str(),db_server_port,ret);
		return false;
	}
	printf("Connect to DBServer[%s:%d] suc.==ret===%d", db_server_ip.c_str(), db_server_port,ret);

	return true;
}

bool LoginModule::ConnectToGlobalServer()
{
	std::string global_server_ip = "127.0.0.1";
	uint16_t global_server_port  = 8002;

	int ret = m_network->Connect(global_server_ip.c_str(), global_server_port);
	if (ret < 0)
	{
		printf("Connect to GlobalServer[%s:%d] Fail!==ret==%d", global_server_ip.c_str(),global_server_port,ret);
		return false;
	}
	printf("Connect to GlobalServer[%s:%d] suc.==ret===%d", global_server_ip.c_str(), global_server_port,ret);

	return true;
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


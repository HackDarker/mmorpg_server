
#include "../common/util_time.h"
#include "../networklib/socketMgr.h"
#include "gatewayModule.h"

class ServerNetworkCallback : public IEngineNetCallback
{
public:
	ServerNetworkCallback(GatewayModule *gateway):m_gateway(gateway),  m_cur_connect_num(0){}
	virtual ~ServerNetworkCallback(){}
	virtual void OnAccept(NetID netid, std::string ip, uint16_t port)
	{
		printf("Gateway OnAccept netid is ============%u\n", netid);
		m_cur_connect_num++;

		if (netid >= m_gateway->m_user_size)
		{
			while (netid >= m_gateway->m_user_size)
			{
				m_gateway->ResizeUserList(m_gateway->m_user_size * 2);
			}
		}
		m_gateway->m_user_list[netid].netid = netid;
		m_gateway->m_user_list[netid].ip = ip;
		m_gateway->m_user_list[netid].last_active_time = m_gateway->m_current_time;
	}
	virtual void OnRecv(NetID netid,WorldPacket* packet)
	{
		printf("gameway OnRecv from netid ===========%u\n",netid);
		if (netid < m_gateway->m_user_size)
		{
			m_gateway->OnRecvUserMsg(netid, packet);
			m_gateway->m_user_list[netid].last_active_time = m_gateway->m_current_time;
		}
	}
	virtual void OnDisconnect(NetID netid)
	{
		printf("Network OnDisconnect netid:%d.", netid);

		--m_cur_connect_num;
		if (netid < m_gateway->m_user_size)
		{
			m_gateway->OnUserDisconnect(netid);
			m_gateway->m_user_list[netid].Reset();
		}
	}
private:
	GatewayModule *m_gateway;

	int m_cur_connect_num;
};

class ServerInternalNetCallback : public IEngineNetCallback
{
public:
	ServerInternalNetCallback(GatewayModule *gateway):m_gateway(gateway){}
	virtual ~ServerInternalNetCallback(){}
	virtual void OnAccept(NetID netid, std::string ip, uint16_t port)
	{
		printf("Internal Network OnAccept netid:%u.%s,%u", netid,ip.c_str(),port);
	}
	virtual void OnRecv(NetID netid, WorldPacket* packet)
	{
		if (m_gateway->m_loginClient.netId == netid){
			m_gateway->OnRecvLoginServerMsg(packet);
		}

	}
	virtual void OnDisconnect(NetID netid)
	{
		printf("OnDisconnect=================%u\n", netid);
	}
private:
	GatewayModule *m_gateway;
};

GatewayModule::GatewayModule()
:m_network(0), m_network_callback(0),m_internal_network_callback(0),
m_current_time(0),m_user_list(0), m_user_size(0)
{
	m_network = SocketMgr::Instance();
	m_network_callback = new ServerNetworkCallback(this);
	m_internal_network_callback = new ServerInternalNetCallback(this);

	m_loginClient.type       = CLIENT_TYPE_LOGIN;
	m_loginClient.serverIp   = "127.0.0.1";
	m_loginClient.serverPort = 8001;

	m_gameClient.type        = CLIENT_TYPE_GAME;
	m_gameClient.serverIp    = "127.0.0.1";
	m_gameClient.serverPort  = 8002;

	ResizeUserList(1);
}

GatewayModule::~GatewayModule()
{
	delete m_network_callback;
	delete m_internal_network_callback;
	free(m_user_list);
}

int GatewayModule::Init()
{
	m_network->RegisterCallback(SOCKET_TYPE_CLIENT,m_network_callback);
	m_network->RegisterCallback(SOCKET_TYPE_INTER,m_internal_network_callback);

	return true;
}

int GatewayModule::Start()
{
	if (!ConnectToLoginServer()){
		return false;
	}

	if (!ConnectToGlobalServer()){
		return false;
	}

	if (!ListenForUser()){
		return false;
	}

	return true;
}

int GatewayModule::Update(uint32_t loopcounter)
{
	m_current_time = getFrameTime();

	//printf("now time is %u\n", m_current_time);
	if(!(loopcounter % 200)) { // 10 second
		if(m_loginClient.retryTime && m_current_time > m_loginClient.retryTime){
			ConnectToLoginServer();
		}
		if(m_gameClient.retryTime && m_current_time > m_gameClient.retryTime){
			ConnectToGlobalServer();
		}
	}
	
	return true;
}

int GatewayModule::Stop()
{
	return true;
}

bool GatewayModule::ConnectToLoginServer()
{
	int netid = m_network->Connect(m_loginClient.serverIp.c_str(), m_loginClient.serverPort);
	if (netid < 0){
		m_loginClient->retryTime = m_current_time + RETRY_CONNECT_TIME;
		printf("Connect to GlobalServer[%s:%d] Fail!==ret==%d", m_loginClient.serverIp.c_str(),m_loginClient.serverPort,ret);
		return false;
	}
	m_loginClient.retryTime = 0;
	m_loginClient.netId = netid;

	RegisterToLogin();

	printf("Connect to GlobalServer[%s:%d] suc.==ret===%d", m_loginClient.serverIp.c_str(), m_loginClient.serverPort,ret);
	return true;
}

bool GatewayModule::ConnectToGlobalServer()
{
	int netid = m_network->Connect(m_gameClient.serverIp.c_str(), m_gameClient.serverPort);
	if (netid < 0){
		m_gameClient->retryTime = m_current_time + RETRY_CONNECT_TIME;
		printf("Connect to GlobalServer[%s:%d] Fail!==ret==%d", m_gameClient.serverIp.c_str(),m_gameClient.serverPort,ret);
		return false;
	}
	m_gameClient.retryTime = 0;
	m_gameClient.netId = netid;

	RegisterToGlobalServer();

	printf("Connect to GlobalServer[%s:%d] suc.==ret===%d", m_gameClient.serverIp.c_str(), m_gameClient.serverPort,ret);
	return true;
}

bool GatewayModule::ListenForUser()
{
	std::string hostname = "127.0.0.1";
	uint16_t listen_port = 8000;
	int ret = m_network->Listen(hostname.c_str(), listen_port);
	if (ret < 0)
	{
		printf("NetListen return FAIL!====ret===%d",ret);
		return false;
	}

	return true;
}

bool GatewayModule::RegisterToLogin()
{
	return true;
}

bool GatewayModule::RegisterToGlobalServer()
{
	return true;
}

void GatewayModule::ResizeUserList(unsigned int size)
{
	if (m_user_size >= size) return;
	
	m_user_list = (User *)realloc(m_user_list, size * sizeof(User));
	for (uint32_t i = m_user_size; i < size; ++i)
	{
		m_user_list[i].Reset();
	}
	m_user_size = size;
}

void GatewayModule::OnRecvUserMsg(NetID netid, const WorldPacket* packet)
{
	printf("OnRecvUserMsg=====id==type===size===%u\n", netid,packet->GetOpcode(),packet->size());
}

void GatewayModule::OnUserDisconnect(NetID netid)
{
	printf("OnUserDisconnect=========%u\n", netid);
}

void GatewayModule::OnRecvLoginServerMsg(const WorldPacket* packet)
{
	printf("OnRecvLoginServerMsg=====id==type===size===%u\n",packet->GetOpcode(),packet->size());
}




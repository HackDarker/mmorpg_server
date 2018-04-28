#include "../common/util_time.h"
#include "../common/imodule.h"
#include "../networklib/socketMgr.h"
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
	virtual void OnRecv(NetID netid, WorldPacket* netPacket)
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
	m_network = SocketMgr::Instance();
	m_internal_network_callback = new ServerInternalNetCallback(this);
	m_current_time = 0;

	m_databaseClient.type       = CLIENT_TYPE_DATABASE;
	m_databaseClient.serverIp   = "127.0.0.1";
	m_databaseClient.serverPort = 8001;
}

GlobalModule::~GlobalModule()
{
	delete m_internal_network_callback;
	free(m_commserver_list);
}

int GlobalModule::Init()
{
	m_network->RegisterCallback(SOCKET_TYPE_INTER,m_internal_network_callback);

	return true;
}

int GlobalModule::Start()
{
	if (!ListenForCommserver()){
		return false;
	}

	if (!ConnectToDbServer()){
		return false;
	}

	return true;
}

int GlobalModule::Update(uint32_t loopcounter)
{
	m_current_time = getFrameTime();
	//printf("now time is %u\n", m_current_time);

	if(!(loopcounter % 200)) { // 10 second
		if(m_databaseClient.retryTime && m_current_time > m_databaseClient.retryTime){
			ConnectToDbServer();
		}
	}
}

int GlobalModule::Stop()
{
	return true;
}

bool GlobalModule::ListenForCommserver()
{
	std::string host_ip = "0.0.0.0";
	uint16_t listen_port = 8002;
	int ret = m_network->Listen(host_ip.c_str(),listen_port);
	if (ret < 0)
	{
		printf("NetListen return FAIL==ret==%d\n", ret);
		return false;
	}
	printf("ListenForCommserver begin=======on====port:%u==%d\n", listen_port,ret);
	
	return true;
}

bool GlobalModule::ConnectToDbServer()
{
	int netid = m_network->Connect(m_databaseClient.serverIp.c_str(), m_databaseClient.serverPort);
	if (netid < 0){
		printf("Connect to DBServer[%s:%d] Fail!==ret==%d", m_databaseClient.serverIp.c_str(),m_databaseClient.serverPort,ret);
		m_databaseClient->retryTime = m_current_time + RETRY_CONNECT_TIME;
		return false;
	}
	m_databaseClient.retryTime = 0;
	m_databaseClient.netId = netid;

	RegisterToDbServer();
	printf("Connect to DBServer[%s:%d] suc.==ret===%d", m_databaseClient.serverIp.c_str(), m_databaseClient.serverPort,ret);
	return true;
}

void GlobalModule::RegisterToDbServer()
{
	WorldPacket data(C_S2S_REGISTER, 4);
	data << (uint8_t)SERVER_TYPE_MAP;

	m_network->SendPacket(m_databaseClient.netId,&data);
}

void GlobalModule::ResizeCommserverList(uint32_t size)
{
	if (m_commserver_size >= size) return;
	
	m_commserver_list = (Commserver *)realloc(m_commserver_list, size * sizeof(Commserver));
	for (uint32_t i = m_commserver_size; i < size; i++)
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


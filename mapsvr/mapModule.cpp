#include "../common/util_time.h"
#include "../common/imodule.h"
#include "../common/commonOpcode.h"
#include "../networklib/socketMgr.h"
#include "mapModule.h"

#define RETRY_CONNECT_TIME 10 //seconds

class ServerInternalNetCallback : public IEngineNetCallback
{
public:
	ServerInternalNetCallback(MapModule* mapsvr):m_mapsvr(mapsvr){}
	virtual ~ServerInternalNetCallback(){}
	virtual void OnAccept(NetID netid, std::string ip, uint16_t port)
	{
		printf("Internal Network OnAccept netid:%u.%s,%u", netid,ip.c_str(),port);
	}
	virtual void OnRecv(NetID netid, WorldPacket* packet)
	{
		if (m_mapsvr->m_gameClient.netId == netid){
			return m_mapsvr->OnRecvDbServerMsg(packet);
		}else if (m_mapsvr->m_databaseClient.netId == netid){
			return m_mapsvr->OnRecvGlobalServerMsg(packet);
		}else{
			printf("Internal Network OnRecv unkown netid:%u", netid);
		}
	}
	virtual void OnDisconnect(NetID netid)
	{
		printf("OnDisconnect=================%u\n", netid);
	}
private:
	MapModule *m_mapsvr;
};

MapModule::MapModule()
{	
	m_current_time = 0;
	m_network = SocketMgr::Instance();
	m_internal_network_callback = new ServerInternalNetCallback(this);
	
	m_databaseClient.type       = CLIENT_TYPE_DATABASE;
	m_databaseClient.serverIp   = "127.0.0.1";
	m_databaseClient.serverPort = 8001;

	m_gameClient.type       = CLIENT_TYPE_GAME;
	m_gameClient.serverIp   = "127.0.0.1";
	m_gameClient.serverPort = 8002;
}

MapModule::~MapModule()
{
	delete m_internal_network_callback;
}

int MapModule::Init()
{
	m_network->RegisterCallback(SOCKET_TYPE_INTER,m_internal_network_callback);

	return true;
}

int MapModule::Start()
{
	if (!ConnectToGlobalServer()){
		return false;
	}
	if (!ConnectToDbServer()){
		return false;
	}

	return true;
}

int MapModule::Update(uint32_t loopcounter)
{
	m_current_time = getFrameTime();

	//printf("now time is %u\n", m_current_time);
	if(!(loopcounter % 200)) { // 10 second
		if(m_databaseClient.retryTime && m_current_time > m_databaseClient.retryTime){
			ConnectToDbServer();
		}
		if(m_gameClient.retryTime && m_current_time > m_gameClient.retryTime){
			ConnectToGlobalServer();
		}
	}
	
	return true;
}

int MapModule::Stop()
{
	return true;
}

bool MapModule::ConnectToGlobalServer()
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

bool MapModule::ConnectToDbServer()
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

void MapModule::RegisterToGlobalServer()
{
	return true;
}

void MapModule::RegisterToDbServer()
{
	WorldPacket data(C_S2S_REGISTER, 4);
	data << (uint8_t)SERVER_TYPE_MAP;

	m_network->SendPacket(m_databaseClient.netId,&data);
}

void MapModule::OnRecvGlobalServerMsg(const WorldPacket* packet)
{
	printf("OnRecvGlobalServerMsg=====id==type===size===%u\n", packet->GetOpcode(),packet->size());
}

void MapModule::OnRecvDbServerMsg(const WorldPacket* packet)
{
	printf("OnRecvLoginServerMsg=====id==type===size===%u\n",packet->GetOpcode(),packet->size());
}






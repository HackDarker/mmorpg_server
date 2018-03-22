
#include "../common/util_time.h"
#include "../common/imodule.h"
#include "../networklib/socketMgr.h"
#include "mapModule.h"

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
		if (m_mapsvr->m_db_server_id == netid){
			return m_mapsvr->OnRecvDbServerMsg(packet);
		}else if (m_mapsvr->m_global_server_id == netid){
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
	m_network = SocketMgr::Instance();
	m_internal_network_callback = new ServerInternalNetCallback(this);
	m_current_time = 0;
	m_global_server_id = 0;
	m_db_server_id = 0;
}

MapModule::~MapModule()
{
	delete m_internal_network_callback;
}

int MapModule::Init()
{
	return true;
}

int MapModule::Start()
{
	m_network->RegisterCallback(SOCKET_TYPE_INTER,m_internal_network_callback);

	if (!ConnectToGlobalServer())
	{
		return false;
	}

	if (!ConnectToDbServer())
	{
		return false;
	}

	return true;
}

int MapModule::Update()
{
	m_current_time = getFrameTime();

	printf("now time is %u\n", m_current_time);
	
	return true;
}

int MapModule::Stop()
{
	return true;
}

bool MapModule::ConnectToGlobalServer()
{
	std::string global_server_ip = "127.0.0.1";
	uint16_t global_server_port  = 8001;

	int netId = m_network->Connect(global_server_ip.c_str(), global_server_port);
	if (netId < 0)
	{
		printf("Connect to GlobalServer[%s:%d] Fail!==ret==%d", global_server_ip.c_str(),global_server_port,netId);
		return false;
	}
	printf("Connect to GlobalServer[%s:%d] suc.==ret===%d", global_server_ip.c_str(), global_server_port,netId);

	m_global_server_id = netId;

	return true;
}

bool MapModule::ConnectToDbServer()
{
	std::string db_server_ip = "127.0.0.1";
	uint16_t db_server_port  = 8001;

	int netId = m_network->Connect(db_server_ip.c_str(), db_server_port);
	if (netId < 0)
	{
		printf("Connect to DbServer[%s:%d] Fail!==ret==%d", db_server_ip.c_str(),db_server_port,netId);
		return false;
	}
	printf("Connect to DbServer[%s:%d] suc.==ret===%d", db_server_ip.c_str(), db_server_port,netId);

	m_db_server_id = netId;

	return true;
}

bool MapModule::RegisterToGlobal()
{
	return true;
}

bool MapModule::RegisterToDatabase()
{
	return true;
}

void MapModule::OnRecvGlobalServerMsg(const WorldPacket* packet)
{
	printf("OnRecvGlobalServerMsg=====id==type===size===%u\n", packet->GetOpcode(),packet->size());
}

void MapModule::OnRecvDbServerMsg(const WorldPacket* packet)
{
	printf("OnRecvLoginServerMsg=====id==type===size===%u\n",packet->GetOpcode(),packet->size());
}






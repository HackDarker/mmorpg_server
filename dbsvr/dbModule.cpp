#include "../networklib/socketMgr.h"
#include "../common/imodule.h"
#include "../common/util_time.h"
#include "dbModule.h"
#include "commonServer.h"
#include <string>

class ServerInternalNetCallback : public IEngineNetCallback
{
public:
	ServerInternalNetCallback(DbModule *dbsvr):m_dbsvr(dbsvr),m_cur_connect_num(0){}
	virtual ~ServerInternalNetCallback(){}
	virtual void OnAccept(NetID netid, std::string ip, uint16_t port)
	{
		m_cur_connect_num++;
		printf("DbSvr OnAccept netid is ==%u==ip is==%s==port is===%u====\n", netid,ip.c_str(),port);

		if (netid >= MAX_COMMSERVER_NUM)
		{
			printf("Dbsvr OnAccept out of max %u\n", netid);
			return;
		}
		if (m_dbsvr->m_commserver_list[netid])
		{
			printf("Dbsvr OnAccept netid is exist %u\n", netid);
			return;
		}
		m_dbsvr->m_commserver_list[netid] = new CommServer();
		m_dbsvr->m_commserver_list[netid]->SetServerInfo(netid,ip,port,m_dbsvr->m_current_time);
	}
	virtual void OnRecv(NetID netid, WorldPacket* netPacket)
	{
		printf("OnRecv=============================%d\n", netid);
		if (netid >= MAX_COMMSERVER_NUM)
		{
			printf("Dbsvr OnRecv out of max %u\n", netid);
			return;
		}
		m_dbsvr->OnRecvCommserverMsg(netid, netPacket);
	}
	virtual void OnDisconnect(NetID netid)
	{
		printf("OnDisconnect=================\n", netid);
		--m_cur_connect_num;
	}
private:
	DbModule *m_dbsvr;

	int m_cur_connect_num;
};

DbModule::DbModule()
{	
	m_current_time = 0;
	m_network = SocketMgr::Instance();
	m_internal_network_callback = new ServerInternalNetCallback(this);
	memset(m_commserver_list,0,sizeof(void *) * MAX_COMMSERVER_NUM);
}

DbModule::~DbModule()
{
	delete m_internal_network_callback;
}

int DbModule::Init()
{
	return true;
}

int DbModule::Start()
{
	m_network->RegisterCallback(SOCKET_TYPE_INTER,m_internal_network_callback);

	if (!ListenCommserver()){
		return false;
	}

	return true;
}

int DbModule::Update()
{
	m_current_time = getFrameTime();
	//printf("now time is %u\n", m_current_time);
	for (int i = 0; i < MAX_COMMSERVER_NUM; ++i)
	{
		if (m_commserver_list[i]){
			m_commserver_list[i]->Update();
		}
	}
}

int DbModule::Stop()
{
	return true;
}

bool DbModule::ListenCommserver()
{
	std::string host_ip = "0.0.0.0";
	uint16_t listen_port = 8001;
	int ret = m_network->Listen(host_ip.c_str(),listen_port);
	if (ret < 0)
	{
		printf("NetListen return FAIL==ret==%d\n", ret);
		return false;
	}

	printf("ListenForGateWay begin=======on====port:%u\n", listen_port);
	return true;
}

void DbModule::OnRecvCommserverMsg(NetID netid, WorldPacket* packet)
{
	printf("OnRecvCommserverMsg=================from netid %u===opcode is %u size is %u===addr%p==\n", netid,packet->GetOpcode(),packet->size(),packet);
	if (!m_commserver_list[netid])
	{
		printf("Dbsvr OnRecv netid is not exist %u\n", netid);
		return;
	}
	
	m_commserver_list[netid]->QueueRecvPacket(packet);
}

void DbModule::OnCommserverDisconnect(NetID netid)
{
	printf("OnGateWayDisconnect====netid is %u\n", netid);
}


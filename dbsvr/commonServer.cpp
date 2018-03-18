#include "../common/commonOpcode.h"
#include "../common/util_time.h"
#include "commonServer.h"
#include "luaProcMgr.h"

CommServer::CommServer()
{
	m_netid = 0;
	m_ip = "";
	m_port = 0;
	m_server_type = 0;
	m_lastPing = 0;
	m_checked = false;
}

CommServer::~CommServer()
{

}

void CommServer::SetServerInfo(NetID netId,std::string ip,uint16_t port,uint32_t nowTime)
{
	m_netid = netId;
	m_ip = ip;
	m_port = port;
	m_lastPing = nowTime;
}

void CommServer::QueueRecvPacket(WorldPacket* packet)
{
	printf("opcode is %u size is %u00000000000000\n", packet->GetOpcode(),packet->size());
	m_recvPacketQueue.Push(packet);
	// MsgPacket* p = m_recvPacketQueue.Pop();
	// printf("opcode is %u size is %uccccccccccccccccc\n", p->GetOpcode(),p->Size());
	m_lastPing = getFrameTime();
}

int CommServer::Update()
{
	WorldPacket* packet;
	while (packet = m_recvPacketQueue.Pop())
	{
		uint16_t op = packet->GetOpcode();
		printf("opcode is %u size is %u1111111111111\n", packet->GetOpcode(),packet->size());
		switch(op){
			case C_S2S_PING:
				HandlePing(packet);break;
			case C_S2S_REGISTER:
				HandleRegister(packet);break;
			case C_S2S_QUERY:
				HandleQuery(packet);break;
			case C_S2S_EXEC:
				HandleExec(packet);break;
			default:
				HandleQuery(packet);
				break;
		}

		//delete packet;
	}
}

void CommServer::HandlePing(WorldPacket* packet)
{
	
}

void CommServer::HandleRegister(WorldPacket* packet)
{

}

void CommServer::HandleQuery(WorldPacket* packet)
{
	//new 一个新包出来存进去
	LuaProcMgr::Instance()->AddPacket(packet);
}

void CommServer::HandleExec(WorldPacket* packet)
{

}

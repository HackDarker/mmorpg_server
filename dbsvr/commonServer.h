#ifndef COMMONSERVER_H
#define COMMONSERVER_H

#include "../common/common.h"
#include "../common/fastQueue.h"
#include "../threadlib/mutex.h"
#include "../networklib/netPacket.h"
#include <string>

class CommServer
{
public:
	CommServer();
	~CommServer();

	void SetServerInfo(NetID netId,std::string ip,uint16_t port,uint32_t nowTime);
	void QueueRecvPacket(MsgPacket* packet);
	int  Update();
private:
	void HandlePing(MsgPacket* packet);
	void HandleRegister(MsgPacket* packet);
	void HandleQuery(MsgPacket* packet);
	void HandleExec(MsgPacket* packet);
private:
	NetID	 m_netid;
	std::string	 m_ip;
	uint16_t m_port;
	uint16_t m_server_type;
	uint32_t m_lastPing;
	bool	 m_checked;

	FastQueue<MsgPacket*, Mutex> m_recvPacketQueue;
};

#endif
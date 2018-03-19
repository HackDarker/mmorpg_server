#ifndef COMMONSERVER_H
#define COMMONSERVER_H

#include "../common/common.h"
#include "../common/fastQueue.h"
#include "../threadlib/mutex.h"
#include "../networklib/worldPacket.h"
#include <string>

class CommServer
{
public:
	CommServer();
	~CommServer();

	void SetServerInfo(NetID netId,std::string ip,uint16_t port,uint32_t nowTime);
	void QueueRecvPacket(WorldPacket* packet);
	void Update();
private:
	void HandlePing(WorldPacket* packet);
	void HandleRegister(WorldPacket* packet);
	void HandleQuery(WorldPacket* packet);
	void HandleExec(WorldPacket* packet);
private:
	NetID	 m_netid;
	std::string	 m_ip;
	uint16_t m_port;
	uint16_t m_server_type;
	uint32_t m_lastPing;
	bool	 m_checked;

	FastQueue<WorldPacket*, Mutex> m_recvPacketQueue;
};

#endif
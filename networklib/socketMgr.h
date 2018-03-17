#ifndef _SOCKETMGR_H_
#define _SOCKETMGR_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <queue>
#include <list>
#include "socket.h"
#include "../threadlib/thread.h"

#define MAX_SOCKET_NUM 20000

#define SOCKET_TYPE_CLIENT 1
#define SOCKET_TYPE_INTER  2

class MsgPacket;

class IEngineNetCallback
{
public:
	virtual ~IEngineNetCallback(){}

	virtual void OnAccept(NetID netid, std::string ip, uint16_t port) = 0;
	virtual void OnRecv(NetID netid, MsgPacket* packet) = 0;
	virtual void OnDisconnect(NetID netid) = 0;
};

class SocketMgr
{
public:
	int m_epoll;
	NetID    m_used_id;
    Socket*  m_sockets[MAX_SOCKET_NUM];
    std::list<NetID>  m_free_sockets;
    IEngineNetCallback* m_net_callback;
    IEngineNetCallback* m_inter_net_callback;
public:
	~SocketMgr();
	static SocketMgr* Instance()
	{
		static SocketMgr inst;
		return &inst;
	}

	Socket* GetSocket(int id)
	{
		return m_sockets[id];
	}

	void Startup();

	int Listen(const char* ip,const uint16_t port);
	int Connect(const char* ip,int port);
	void AddSocket(Socket *s);
	void RemoveSocket(Socket *s);
	void CloseAll();

	NetID AllocSocketId();
	int ModSocketEvent(Socket* s, int w);
	int SetNoblock(int fd);

	void SendPacket(int fd,MsgPacket* packet);
	void RegisterCallback(uint16_t type,IEngineNetCallback* call_back);
private:
	SocketMgr();
};

class SocketWorkerThread : public ThreadBase
{
    struct epoll_event events[MAX_SOCKET_NUM];
    bool running;
public:
    bool run();
    void OnShutdown()
    {
        running = false;
    }
};

#endif
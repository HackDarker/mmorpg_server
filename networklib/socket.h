#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "buffer.h"
#include "../threadlib/mutex.h"
#include "worldPacket.h"

enum SocketStatus
{
	SO_LISTEN = 1 << 0,
	SO_CONNECT = 1 << 1,
	SO_WRITABLE = 1 << 2,
};

class Socket
{
public:
	Socket(int fd,int type,uint32_t sendBuffSize = 1024*1024,uint32_t recvBuffSize = 1024*1024);
	~Socket();

	void Send(uint32_t state);
	void SetId(int id);
	uint16_t GetId();
	int  GetFd() {return m_fd;}
	void SetState(uint32_t state);
	uint32_t HasState(int state);
	void Disconnect();

	void OnAccept();
	void SendPacket(const WorldPacket* packet);
	void OutPacket(uint16_t opcode, size_t len, const void* data);
	void WriteCallback();
	void ReadCallback();
	void OnRead();

    void LockSendBuff()   { m_sendMutex.Lock(); }
    void UnlockSendBuff() { m_sendMutex.UnLock();}
    void LockRecvBuff()   { m_recvMutex.Lock();}
    void UnlockRecvBuff() { m_recvMutex.UnLock();}
    Buffer& GetSendBuff() { return m_sendBuff; }
    Buffer& GetRecvBuff() { return m_recvBuff;}
private:
	int m_fd;
	uint16_t m_id;
	uint16_t m_type;
	uint32_t m_state;
	Mutex m_sendMutex;
	Mutex m_recvMutex;
	Buffer m_sendBuff;
	Buffer m_recvBuff;

	uint16_t m_opcode;
	uint16_t m_remaining;
};

#endif
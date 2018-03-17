#include "socketMgr.h"
#include "socket.h"

Socket::Socket(int fd,int type,uint32_t sendBuffSize,uint32_t recvBuffSize)
{
	m_fd = fd;
	m_type = type;
	m_state = 0;
	m_sendBuff.BuffExpand(sendBuffSize);
	m_recvBuff.BuffExpand(recvBuffSize);
	m_opcode = 0;
	m_remaining = 0;
}

Socket::~Socket()
{

}

void Socket::SetId(int id)
{
	m_id = id;
}

uint16_t Socket::GetId()
{
	return m_id;
}

void Socket::SetState(uint32_t state)
{
	m_state |= state;
}

uint32_t Socket::HasState(int state)
{
	return m_state & state;
}

void Socket::OnAccept()
{
	int new_fd;
	uint32_t len; 
	struct sockaddr_in clientAddr;
	memset(&clientAddr,0,sizeof(clientAddr));

	while(true)
	{
		new_fd = accept(m_fd, (sockaddr*)&clientAddr, (socklen_t*)&len);
		if(new_fd == -1)
		{
			if(errno == EINTR) continue;
			return;
		}

		Socket* socketPtr = new Socket(new_fd,SOCKET_TYPE_CLIENT);
		SocketMgr::Instance()->AddSocket(socketPtr);

		char* ip = inet_ntoa(clientAddr.sin_addr);
		uint16_t port = clientAddr.sin_port;
		SocketMgr::Instance()->m_net_callback->OnAccept(socketPtr->GetId(),ip,port);
	}
}

void Socket::Disconnect()
{
	// remove from mgr
	SocketMgr::Instance()->RemoveSocket(this);
	close(m_fd);
}

void Socket::SendPacket(const MsgPacket* packet)
{
	if(!packet) return;

	OutPacket(packet->GetOpcode(), packet->Size(), packet->Contents());
}

void Socket::OutPacket(uint16_t opcode, size_t len, const void* data)
{
	if( len + sizeof(MsgPacketHeader) > 65536 || len <= 0)
	{
		printf("WARNING: Tried to send a packet of %u bytes to a socket. Opcode was: %u (0x%03X)\n", (unsigned int)len, (unsigned int)opcode, (unsigned int)opcode);
		return;
	}

	LockSendBuff();

	MsgPacketHeader header;
	msg_header_set_size(header, len);
	msg_header_set_opcode(header, opcode);

	// Pass the header to our send buffer
	// Pass the rest of the packet to our send buffer (if there is any)
	m_sendBuff.PushData((const char*)&header, (uint32_t)sizeof(header));
	m_sendBuff.PushData((const char*)data, (uint32_t)len);
	
	printf("=====================m_sendBuff.GetCur()==============%u\n", m_sendBuff.GetCur());
	if (m_sendBuff.GetCur() > 0)
	{
		int ret = SocketMgr::Instance()->ModSocketEvent(this,1);
		printf("send (opcode: %u) of %u bytes.ret is %u\n", opcode, (uint32_t)len,ret);
	}

	UnlockSendBuff();
}

void Socket::WriteCallback()
{
	// We should already be locked at this point, so try to push everything out.
    size_t size;
    int bytes_written;
    while(true)
    {
        size = m_sendBuff.GetCur();
        if(size == 0) break;
        bytes_written = send(m_fd, m_sendBuff.GetStart(), size, 0);
        if(bytes_written < 0)
        {
            if(errno == EAGAIN) break;
            if(errno == EINTR) continue;
            // error
            Disconnect();
            break;
        }
        m_sendBuff.BufferReaded((uint32_t)bytes_written);
    }
}

void Socket::ReadCallback()
{
	// We have to lock here.
	printf("11111111111111111111\n");
    LockRecvBuff();
    printf("2222222222222222222222\n");

    int freeSize;
    int bytes;
    int total = 0;
    while(true)
    {
        freeSize = m_recvBuff.GetFreeSize();
        if(freeSize <= 0)
        {
             printf("No enough read buffer space on fd %d", m_fd);
             break;
        }
        bytes = recv(m_fd, m_recvBuff.GetStart(), freeSize, 0);
        if(bytes < 0)
        {
            if(errno == EAGAIN) break;
            if(errno == EINTR) continue;
           	UnlockRecvBuff();
            Disconnect();
            return;
        }
        else if(bytes == 0)
        {
            UnlockRecvBuff();
            Disconnect();
            return;
        }    
        else
        {
            total += bytes;
            m_recvBuff.BufferWrited((uint32_t)bytes);
        }
    }

    printf("====================total============       %u\n", total);
    if(total > 0)
    {
        OnRead();
    }

    UnlockRecvBuff();
}

void Socket::OnRead()
{
	while(true)
	{
		if(!m_remaining)
		{
			if(GetRecvBuff().GetCur() < sizeof(MsgPacketHeader))	{
				return; // No header in the packet, let's wait.
			}
			// Copy from packet buffer into header local var
			//uint8_t* str = (uint8_t*)malloc(8);
			//GetRecvBuff().ReadData(str, 7);

			MsgPacketHeader header;
			GetRecvBuff().ReadData((uint8_t*)&header, sizeof(MsgPacketHeader));

			char* str1 = (char*)&header;

			printf("tttttttt%u %u %u %u====%u\n",str1[0],str1[1],str1[2],str1[3],sizeof(MsgPacketHeader));

			//printf("sizeof(MsgPacketHeader)===== %u %lu %u\n", 
			//sizeof(MsgPacketHeader),header.opcode,header.size);

			m_opcode = msg_header_get_opcode(header);
			m_remaining = msg_header_get_size(header) - sizeof(uint16_t);

			printf("opcode====%d m_remaining %d===cur%d====\n", m_opcode,m_remaining,GetRecvBuff().GetCur());
			if((m_remaining+10) > 65535){
				Disconnect();
				return;
			}
		}

		// do we have a full packet?
		printf("cur and m_remaining=======%d,%d========",GetRecvBuff().GetCur(),m_remaining);
		if(GetRecvBuff().GetCur() < m_remaining){
			return;
		}

		MsgPacket * netPacket = new MsgPacket(m_opcode, m_remaining);
		if(m_remaining){
			GetRecvBuff().ReadData((uint8_t*)netPacket->Contents(), m_remaining);
		}

		switch(m_type){
			case SOCKET_TYPE_CLIENT:
				SocketMgr::Instance()->m_net_callback->OnRecv(m_id,netPacket);
				break;
			case SOCKET_TYPE_INTER:
				SocketMgr::Instance()->m_net_callback->OnRecv(m_id,netPacket);
				break;
			default:
				printf("socket OnRead unkown type %u\n", m_type);
				break;
		}
		printf("received (opcode: %u) of %u bytes.", m_opcode, (uint32_t)m_remaining);

		m_remaining = m_opcode = 0;
	}
}
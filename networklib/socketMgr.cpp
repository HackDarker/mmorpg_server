#include "socketMgr.h"
#include "../threadlib/threadpool.h"
#include <fcntl.h>

SocketMgr::SocketMgr()
{
	m_epoll = epoll_create(MAX_SOCKET_NUM);
	if (m_epoll <= 0)
	{
		printf("epoll error\n");
		exit(-1);
	}	

	m_used_id = 0;
	m_net_callback = 0;
	m_inter_net_callback = 0;
	memset(m_sockets, 0, sizeof(void*) * MAX_SOCKET_NUM);
}

SocketMgr::~SocketMgr()
{
	close(m_epoll);
}

void SocketMgr::CloseAll()
{
	for (int i = 0; i < MAX_SOCKET_NUM; ++i)
	{
		if(m_sockets[i] != NULL)
		{
			m_sockets[i]->Disconnect();
		}
	}
}

int SocketMgr::SetNoblock(int fd)
{
	int flag = fcntl(fd,F_GETFL,0);
	if (-1 == flag) return -1;

	fcntl(fd,F_SETFL,flag | O_NONBLOCK);
	return 0;
}

NetID SocketMgr::AllocSocketId()
{
	NetID id = INVAILD_SOCKETID;
	if (0 != m_free_sockets.size()){
		id = m_free_sockets.front();
		m_free_sockets.pop_front();
	}else if (m_used_id < MAX_SOCKET_NUM){
		id = ++m_used_id;
	}

	return id;
}

void SocketMgr::AddSocket(Socket *s)
{	
	NetID id = AllocSocketId();
	printf("id is =================%d\n",id);
	if (id == INVAILD_SOCKETID){
		printf("epoll the insert fd %u is exist",id);
		return;
	}
	m_sockets[id] = s;
	s->SetId(id);
	SetNoblock(s->GetFd());

	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events |= EPOLLET | EPOLLIN;
	ev.data.u64 = id;
	if (epoll_ctl(m_epoll,EPOLL_CTL_ADD,s->GetFd(),&ev))
		printf("can not add event to epoll set on fd %u\n",s->GetFd());

	printf("=============AddSocket success============= %u\n",s->GetFd());
}

int SocketMgr::ModSocketEvent(Socket* s, int w)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events |= EPOLLET | EPOLLERR | EPOLLHUP | EPOLLIN;
	if (w) 
	{
		printf("===============WWWWWW============",w);
		ev.events |= EPOLLOUT;
	}
		
	ev.data.ptr = s;
	if (epoll_ctl(m_epoll, EPOLL_CTL_MOD, s->GetFd(), &ev)) 
	{
		return -1;
	}
		
	return 0;
}

void SocketMgr::RemoveSocket(Socket *s)
{
	if(m_sockets[s->GetFd()] != s)
	{
		printf("Could not remove fd %u from the set due to it not existing?", s->GetFd());
        return;
	}

	m_sockets[s->GetFd()] = 0;

    // Remove from epoll list.
    struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
    ev.data.fd = s->GetFd();
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP ;

    if(epoll_ctl(m_epoll, EPOLL_CTL_DEL, ev.data.fd, &ev)) {
    	printf("Could not remove fd %u from epoll set, errno %u", s->GetFd(), errno);
    }
}

int SocketMgr::Listen(const char* ip,const uint16_t port)
{
	struct sockaddr_in addr;
	int fd = socket(AF_INET,SOCK_STREAM,0);
	if (fd <= 0)
	{
		return -1;
	}

	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET,ip,&addr.sin_addr);

	if (0 != bind(fd,(struct sockaddr *)&addr,	sizeof(addr)))
	{
		return -3;
	}
	if (0 != listen(fd,128))
	{
		return -4;
	}

	Socket *so = new Socket(fd,SOCKET_TYPE_CLIENT);
	so->SetState(SO_LISTEN);
	AddSocket(so);

	printf("\n================Listen============ip%s==port==%u====fd====%u\n",ip,port,fd);

	return 0;
}

int SocketMgr::Connect(const char* ip,const int port)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	printf("================Connect============%u\n", fd);
	if (fd < 0)	
	{
		return -1;
	}

	if (SetNoblock(fd) < 0)
	{
		close(fd);
		return -2;
	}
	
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;  
	addr.sin_port = htons(port);  
	addr.sin_addr.s_addr = inet_addr(ip);
	errno = 0;
	int ret = connect(fd, (struct sockaddr *)(&addr), sizeof(struct sockaddr));
	if (ret < 0 && errno != EINPROGRESS) 
	{
		close(fd);
		return -3;
	}

	Socket* so = new Socket(fd,SOCKET_TYPE_INTER);
	so->SetState(SO_CONNECT);
	AddSocket(so);

	return so->GetId();
}	

void SocketMgr::Startup()
{
	printf("111111111111111111111111111111\n");
    int count = 1;
    for(int i = 0; i < count; i++){
    	ThreadPool::Instance()->ExecuteTask(new SocketWorkerThread());
    } 
}

void SocketMgr::SendPacket(int netId,WorldPacket* packet)
{
	if(netId >= MAX_SOCKET_NUM || !m_sockets[netId]){
        return;
	}

	m_sockets[netId]->SendPacket(packet);
}

void SocketMgr::RegisterCallback(uint16_t type,IEngineNetCallback* call_back)
{
	switch(type){
		case SOCKET_TYPE_CLIENT:
			m_net_callback = call_back;
			break;
		case SOCKET_TYPE_INTER:
			m_inter_net_callback = call_back;
			break;
		default:
			printf("RegisterCallback unkown type %u\n", type);
			break;
	}
}


bool SocketWorkerThread::run()
{	
	bool running = true;
    int fd_count;
    Socket * ptr;
    running = true;
    SocketMgr* mgr = SocketMgr::Instance();

    printf("SocketWorkerThread run %d\n", running);

    while(running)
    {
        fd_count = epoll_wait(SocketMgr::Instance()->m_epoll, events, MAX_SOCKET_NUM, -1);

        printf("epoll_wait fd_count %d %d %d\n", fd_count);
        for(int i = 0; i < fd_count; i++)
        {
        	printf("netid is ==========%lu=======\n", events[i].data.u64);
            if(events[i].data.u64 >= MAX_SOCKET_NUM){
               printf("Requested FD that is too high (%u)", events[i].data.fd);
               continue;
            }

            ptr = mgr->m_sockets[events[i].data.u64];
            if (ptr && ptr->HasState(SO_LISTEN)){
            	printf("==========Accpet=========%lu===%d\n",events[i].data.u64,events[i].events & EPOLLIN);
            	ptr->OnAccept();//接受连接
				continue;
            }

            if(events[i].events & EPOLLHUP || events[i].events & EPOLLERR){
				printf("=======EPOLLHUP or EPOLLERR====%lu\n",events[i].data.u64);
                continue;
            }else if(events[i].events & EPOLLIN){
            	printf("============EPOLLIN============%lu\n",events[i].data.u64);
                ptr->ReadCallback(); //读数据
            }else if(events[i].events & EPOLLOUT){
            	printf("============EPOLLOUT===========%lu\n",events[i].data.u64);
                ptr->LockSendBuff();
                ptr->WriteCallback();
                if (ptr->GetSendBuff().GetCur() <= 0){
                	mgr->ModSocketEvent(ptr,0);
                }
                ptr->UnlockSendBuff();           
            }
        }       
    }

    return true;
}

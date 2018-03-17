#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>   
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include "../networklib/socketMgr.h"
#include <string>

#define IPADDRESS   "0.0.0.0"
#define PORT        8002
#define MAXSIZE     1024
#define LISTENQ     5
#define FDSIZE      20000
#define EPOLLEVENTS 20000
  
  //函数声明
  //创建套接字并进行绑定
  static int socket_bind(const char* ip,int port);
  //IO多路复用epoll
  static void do_epoll(int listenfd);
  //事件处理函数
  static void
  handle_events(int epollfd,struct epoll_event *events,int num,int listenfd,char *buf);
  //处理接收到的连接
  static void handle_accpet(int epollfd,int listenfd);
  //读处理
  static void do_read(int epollfd,int fd,char *buf);
  //写处理
  static void do_write(int epollfd,int fd,char *buf);
  //添加事件
  static void add_event(int epollfd,int fd,int state);
  //修改事件
  static void modify_event(int epollfd,int fd,int state);
  //删除事件
  static void delete_event(int epollfd,int fd,int state);

  static int Listen(const char* ip,const uint16_t port);
  
  int main(int argc,char *argv[])
  {
      SocketMgr* m_network = SocketMgr::Instance();
      std::string host_ip = "0.0.0.0";
      uint16_t listen_port = 8002;
      int ret = m_network->Listen(host_ip.c_str(),listen_port);
      if (ret < 0)
      {
        printf("NetListen return FAIL==ret==%d\n", ret);
        return false;
      }
      printf("ListenForGateWay begin=======on====port:%u\n", listen_port);
      printf("0000000000000000000000\n");

      
      // int listenfd = Listen(host_ip.c_str(),listen_port);
      // add_event(SocketMgr::Instance()->m_epoll,listenfd,EPOLLIN);

      int fd_count;
      Socket * ptr;
      printf("1111111111111111111111\n");
      SocketMgr* mgr = SocketMgr::Instance();
      printf("2222222222222222222\n");
      struct epoll_event events[MAX_SOCKET_NUM];
      printf("3333333333333333333333333333 ===%d====%d\n",SocketMgr::Instance()->m_epoll,MAX_SOCKET_NUM);

      while(true)
      {
        printf("SocketWorkerThread run %d==%d\n",SocketMgr::Instance()->m_epoll,MAX_SOCKET_NUM);
        fd_count = epoll_wait(SocketMgr::Instance()->m_epoll, events, MAX_SOCKET_NUM, 5000);
        printf("epoll_wait fd_count %d \n", fd_count);
      }

    // std::string host_ip = "0.0.0.0";
    // uint16_t listen_port = 8002;
    // int listenfd = Listen(host_ip.c_str(),listen_port);
    // printf("listenfd====%d===\n",listenfd);
    // do_epoll(listenfd);
    return 0;

    return 0;
}

static int Listen(const char* ip,const uint16_t port)
{
  struct sockaddr_in addr;
  int fd = socket(AF_INET,SOCK_STREAM,0);
  if (fd <= 0)
  {
    return -1;
  }
  // if (SetNoblock(fd) < 0)
  // {
  //  close(fd);
  //  return -2;
  // }

  memset(&addr,0,sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET,ip,&addr.sin_addr);

  if (0 != bind(fd,(struct sockaddr *)&addr,  sizeof(addr)))
  {
    return -3;
  }
  if (0 != listen(fd,128))
  {
    return -4;
  }

  return fd;
}
  
  static int socket_bind(const char* ip,int port)
  {
      int  listenfd;
      struct sockaddr_in servaddr;
      listenfd = socket(AF_INET,SOCK_STREAM,0);
      if (listenfd == -1)
      {
          perror("socket error:");
          exit(1);
      }
      bzero(&servaddr,sizeof(servaddr));
      servaddr.sin_family = AF_INET;
      inet_pton(AF_INET,ip,&servaddr.sin_addr);
      servaddr.sin_port = htons(port);
      if (bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1)
      {
          perror("bind error: ");
          exit(1);
      }
      return listenfd;
  }
  
  static void do_epoll(int listenfd)
  {
      struct epoll_event events[MAX_SOCKET_NUM];
      add_event(SocketMgr::Instance()->m_epoll,listenfd,EPOLLIN);
      // for ( ; ; )
      // {
      //     //获取已经准备好的描述符事件
      //     ret = epoll_wait(epollfd,events,EPOLLEVENTS,-1);
      //     printf("================%d==============1111111\n", ret);
      //     handle_events(epollfd,events,ret,listenfd,buf);
      // }
      int ret;
      while(true)
      {
        //printf("SocketWorkerThread run %d==%d",SocketMgr::Instance()->m_epoll,MAX_SOCKET_NUM);
        ret = epoll_wait(SocketMgr::Instance()->m_epoll, events, MAX_SOCKET_NUM, 5000);
        printf("================%d==============1111111\n", ret);
        //printf("epoll_wait fd_count %d %d %d\n", fd_count,SocketMgr::Instance()->m_epoll,SocketMgr::Instance()->GetSocket(1)->GetFd());
      }

      close(SocketMgr::Instance()->m_epoll);
  }
  
  static void
  handle_events(int epollfd,struct epoll_event *events,int num,int listenfd,char *buf)
  {
      int i;
      int fd;
      //进行选好遍历
      for (i = 0;i < num;i++)
      {
         fd = events[i].data.fd;
         //根据描述符的类型和事件类型进行处理
         if ((fd == listenfd) &&(events[i].events & EPOLLIN))
             handle_accpet(epollfd,listenfd);
         else if (events[i].events & EPOLLIN)
             do_read(epollfd,fd,buf);
         else if (events[i].events & EPOLLOUT)
             do_write(epollfd,fd,buf);
     }
 }
 static void handle_accpet(int epollfd,int listenfd)
 {
     int clifd;
     struct sockaddr_in cliaddr;
     socklen_t  cliaddrlen;
     clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
     if (clifd == -1)
         perror("accpet error:");
     else
     {
         printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
         //添加一个客户描述符和事件
         add_event(epollfd,clifd,EPOLLIN);
     }
 }
 
 static void do_read(int epollfd,int fd,char *buf)
 {
     int nread;
     nread = read(fd,buf,MAXSIZE);
     if (nread == -1)
     {
         perror("read error:");
         close(fd);
         delete_event(epollfd,fd,EPOLLIN);
     }
     else if (nread == 0)
     {
         fprintf(stderr,"client close.\n");
         close(fd);
         delete_event(epollfd,fd,EPOLLIN);
     }
     else
     {
         printf("read message is : %s",buf);
         //修改描述符对应的事件，由读改为写
         modify_event(epollfd,fd,EPOLLOUT);
     }
 }
 
 static void do_write(int epollfd,int fd,char *buf)
 {
     int nwrite;
     nwrite = write(fd,buf,strlen(buf));
     if (nwrite == -1)
     {
         perror("write error:");
         close(fd);
         delete_event(epollfd,fd,EPOLLOUT);
     }
     else
         modify_event(epollfd,fd,EPOLLIN);
     memset(buf,0,MAXSIZE);
 }
 
 static void add_event(int epollfd,int fd,int state)
 {
     struct epoll_event ev;
     ev.events = state;
     ev.data.fd = fd;
     epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
 }
 
 static void delete_event(int epollfd,int fd,int state)
 {
     struct epoll_event ev;
     ev.events = state;
     ev.data.fd = fd;
     epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
 }
 
 static void modify_event(int epollfd,int fd,int state)
 {
     struct epoll_event ev;
     ev.events = state;
     ev.data.fd = fd;
     epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
 }
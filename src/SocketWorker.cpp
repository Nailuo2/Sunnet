#include "SocketWorker.h"
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <Sunnet.h>
#include <fcntl.h>
#include <sys/socket.h>

//初始化
void SocketWorker::Init(){
    cout<<"SocketWorker Init"<<endl;
    //创建epoll对象
    epollFd = epoll_create(1024);   //返回值：非负表成功创建epoll对象，-1失败
    assert(epollFd > 0);

}

//接收新客户端
void SocketWorker::OnAccept(shared_ptr<Conn> conn){
    cout<<"OnAccept fd:" <<conn->fd<<endl;
    //步骤1：accept
    int clientFd = accept(conn->fd,NULL,NULL);
    if(clientFd < 0){
        cout<<"accept error" <<conn->fd<<endl;
    }
    //步骤2：设置非阻塞
    fcntl(clientFd,F_SETFL,O_NONBLOCK);
    //步骤3：添加连接对象
    Sunnet::inst->AddConn(clientFd,conn->serviceId,Conn::TYPE::CLIENT);
    //步骤4：添加到epoll监听列表
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = clientFd;
    if(epoll_ctl(epollFd,EPOLL_CTL_ADD,clientFd,&ev)==-1){
        cout<<"OnAccept epoll_ctl Fail:" <<strerror(errno)<<endl;
    }
    //步骤5：通知服务
    auto msg = make_shared<SocketAcceptMsg>();
    msg->type = BaseMsg::TYPE::SOCKET_ACCEPT;
    msg->listenFd = conn->fd;
    msg->clientFd = clientFd;
    Sunnet::inst->Send(conn->serviceId,msg);
}

//传递可读写事件
void SocketWorker::OnRW(shared_ptr<Conn> conn,bool r,bool w){
    cout<<"OnRW fd:" <<conn->fd<<endl;
    auto msg = make_shared<SocketRWMsg>();
    msg->type = BaseMsg::TYPE::SOCKET_RW;
    msg->fd  = conn->fd;
    msg->isRead = r;
    msg->isWrite = w;
    Sunnet::inst->Send(conn->serviceId,msg);
}

//处理事件
void SocketWorker::OnEvent(epoll_event ev){
    int fd = ev.data.fd;
    auto conn = Sunnet::inst->GetConn(fd);
    if(conn == NULL){
        cout<< "OnEvent error,conn == NULL "<<endl;
        return;
    }
    //事件类型
    bool isRead = ev.events & EPOLLIN;
    bool isWrite = ev.events & EPOLLOUT;
    bool isError = ev.events & EPOLLERR;
    //监听socket
    if(conn->type==Conn::TYPE::LISTEN){
        if(isRead){
            OnAccept(conn);
        }
    }else{  //普通socket
        if(isRead || isWrite){
            OnRW(conn,isRead,isWrite);
        }
        if(isError){
            cout<<"OnError fd:"<<conn->fd<<endl;
        }
    }
}

void SocketWorker::operator()(){
    while(true){
        //阻塞等待
        const int EVENT_SIZE = 64;
        struct epoll_event events[EVENT_SIZE];
        //epoll_wait 参数1：代表要关联的epoll对象结构，2:用于指定epoll_event类型的数组，3：数组长度 4：
        int eventCount =epoll_wait(epollFd,events,EVENT_SIZE,-1); //返回事件个数
        //取得事件
        for(int i=0;i<eventCount;i++){
            epoll_event ev = events[i]; //当前要处理的事件
            OnEvent(ev);    //处理单个事件方法
        }
    }
}


//新增监听事件  
void SocketWorker::AddEvent(int fd){
    cout<<"AddEvent fd "<<fd<<endl;
    //添加到epoll对象
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;  //代表边缘触发模式监听可读事件 2.EPOLLIN|EPOLLOUT|EPOLLET代表边缘触发模式监听可读可写事件
    ev.data.fd = fd;
    //epoll_ctl 是修改epoll对象监听列表方法 参数1：代表要操作的epoll对象 2：执行的操作(增删改) 3:操作的文件描述符（socket）
    //参数4：是一个epoll_event类型结构体，分为events和data两部分，主要用于控制监听行为 events包含 监听什么和触发模式属性
    //
    if(epoll_ctl(epollFd,EPOLL_CTL_ADD,fd,&ev)==-1){
        cout<<"AddEvent epoll_ctl Fail:"<<strerror(errno)<<endl;
    }
}

//删除监听事件
void SocketWorker::RemoveEvent(int fd){
    cout<<"RemoveEvent fd "<<fd<<endl;
    epoll_ctl(epollFd,EPOLL_CTL_DEL,fd,NULL);
}

//修改监听事件方法  EPOLLET代表边缘触发模式  若不添加 则默认水平触发模式
void SocketWorker::ModifyEvent(int fd,bool epollOut){
    cout<<"ModifyEvent fd "<<fd<<" "<<epollOut<<endl;
    struct epoll_event ev;
    ev.data.fd = fd;
    if(epollOut){
        ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
    }else{
        ev.events = EPOLLIN | EPOLLET;
    }
    epoll_ctl(epollFd,EPOLL_CTL_MOD,fd,&ev);
}








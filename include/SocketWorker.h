#pragma once
#include <sys/epoll.h>
#include <memory>
#include "Conn.h"
using namespace std;

class SocketWorker {
private:
    //epoll描述符
    int epollFd;
public:
    void Init();        //初始化
    void operator()();  //线程函数
public:
    //增 删 改 epoll对象监听列表方法
    void AddEvent(int fd);
    void RemoveEvent(int fd);
    void ModifyEvent(int fd,bool epollOut);
private:
    void OnEvent(epoll_event ev);                   // 处理事件
    void OnAccept(shared_ptr<Conn> conn);           //套接字发生可读，调用
    void OnRW(shared_ptr<Conn> conn,bool r,bool w); //套接字发送可读可写，调用
};
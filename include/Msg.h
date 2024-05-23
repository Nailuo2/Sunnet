#pragma once
#include <memory>
using namespace std;

//消息基类
class BaseMsg{
public:
    enum TYPE{          //消息类型
        SERVICE = 1,
        SOCKET_ACCEPT = 2, 
        SOCKET_RW = 3,
     };
    uint8_t type;           //消息类型
    char load[999999]{};    //用于检测内存泄漏，仅用于调试
    virtual ~BaseMsg(){};   //虚析构   当一个类有子类时，它的析构函数必须是虚析构，否则在多态情况下删除对象可能造成内存泄漏，反之代价占用更多内存
};

//服务间消息
class ServiceMsg : public BaseMsg {
public:
    uint32_t source;        //消息发送方
    shared_ptr<char> buff;  //消息内容
    size_t size;            //消息内容大小
};

//有新的客户端连接
class SocketAcceptMsg : public BaseMsg {
public:
    int listenFd;   //监听套接字描述符
    int clientFd;   //新连接客户端的套接字描述符
};

//某个客户端可读可写  
class SocketRWMsg : public BaseMsg {
public:
    int fd;                 //发送事件的套接字描述符
    bool isRead = false;    //真代表可读
    bool isWrite = false;   //真代表可写
};



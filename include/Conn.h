#pragma once
using namespace std;

class Conn{
public:
    enum TYPE{  //消息类型
    //从服务端角度看套接字分为 监听和普通 套接字 。
    //服务端开启监听，操作系统会激活一个监听套接字，专门接收客户端的连接
    //当服务端接收(accept)连接时，操作系统会创建一个与客户端对应的普通套接字
        LISTEN = 1, //监听套接字
        CLIENT = 2, //普通套接字，每个普通套接字对应一个客户端
    };

    uint8_t type;       //套接字类型
    int fd;             //套接字描述符
    uint32_t serviceId; //代表与套接字fd关联的服务
};
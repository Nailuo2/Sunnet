#pragma once
#include "Worker.h"
#include "Service.h"
#include "SocketWorker.h"
#include "Conn.h"
#include <vector>
#include <unordered_map>
#include <string>


class Worker;//相互引用

class Sunnet{
public:
    //单例 
    static Sunnet* inst;//"声明"   在构造函数(创建对象时会被调用)中会给它赋值
public:
    //构造函数
    Sunnet();
    //初始化并开始
    void Start();
    //等待运行
    void Wait();
    //增删服务
    uint32_t NewService(shared_ptr<string> type);
    void KillService(uint32_t id);  //仅限服务自己调用
    //发送消息
    void Send(uint32_t toId, shared_ptr<BaseMsg> msg);
    //全局队列操作
    shared_ptr<Service> PopGlobalQueue();
    void PushGlobalQueue(shared_ptr<Service> srv);
    //让工作线程等待（仅工作线程调用）
    void WorkerWait();
    
    shared_ptr<BaseMsg> MakeMsg(uint32_t source,char* buff,int len);

    //增删查Conn
    int AddConn(int fd,uint32_t id,Conn::TYPE type);
    shared_ptr<Conn> GetConn(int fd);
    bool RemoveConn(int fd);

    //网络连接操作接口  
    int Listen(uint32_t port,uint32_t serviceId);//port:代表要监听的端口 serviceId代表所关联的服务id
    void CloseConn(uint32_t fd);    //关闭连接
    //对外Event接口
    void ModifyEvent(int fd, bool epollOut);
private:
    //工作线程
    int WORKER_NUM = 3;             //工作线程数（配置）
    vector<Worker*> workers;        //worker 对象
    vector<thread*> workerThreads;  //线程

    //Socket线程
    SocketWorker* socketWorker;
    thread* socketThread;

    //服务列表
    unordered_map<uint32_t,shared_ptr<Service>> services;
    uint32_t maxId = 0;             //最大ID
    pthread_rwlock_t servicesLock;   //读写锁
    //全局队列 存有待处理消息的服务
    queue<shared_ptr<Service>> globalQueue;
    int globalLen = 0;               //队列长度
    pthread_spinlock_t globalLock;   //自旋锁
    //休眠和唤醒
    pthread_mutex_t sleepMtx;
    pthread_cond_t sleepCond;
    //休眠工作线程数
    int sleepCount = 0;

    //Conn列表
    unordered_map<uint32_t,shared_ptr<Conn>> conns;
    pthread_rwlock_t connsLock;     //读写锁
private:
    //开启工作线程
    void StartWorker();
    //唤醒工作线程
    void CheckAndWeakUp();
    //开启Socket线程 用于初始化网络功能
    void StartSocket();
    //获取服务
    shared_ptr<Service> GetService(uint32_t id);
};
#pragma once
#include "Sunnet.h"
#include "Service.h"
#include <thread>

class Sunnet;   //Sunnet前向声明，解决循环引用问题
using namespace std;

class Worker{
public:
    int id;         //编号
    int eachNum;    //每次处理多少条数据
    void operator()();//线程函数    "operator()()"重载括号操作符
private:
    //辅助函数
    void CheckAndPutGlobal(shared_ptr<Service> srv);
};

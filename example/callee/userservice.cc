#include <iostream>
#include <string>

#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

/*
调试过程中如果已经有启动的服务
ps -u  查看进程
kill -9 xxx   xxx为对应进程号 
ps -u
重新启动服务
*/

/*
UserService原来是一个本地服务，提供了两个进程的本地方法，Login和GetFriendLists
*/
//using namespace fixbug;// 不包括这个找不到UserServiceRpc

class UserService : public fixbug::UserServiceRpc// 使用在rpc服务发布端（rpc服务提供者）
{

public:
    bool Login(std::string name,std::string pwd){
        std::cout<< "doing local service :Login"<<std::endl;
        std::cout<< "name" << name<<" pwd:"<<pwd<<std::endl;
        return true; // true 最后login success:1  false 最后login success:0
    }

    bool Register(uint32_t id,std::string name,std::string pwd){
        std::cout<< "doing local service :Login"<<std::endl;
        std::cout<< "name" << name<<" pwd:"<<pwd<<std::endl;
        return true; 
    }


    // 重写基类UserServiceRpc的虚函数 下面这些方法都是框架直接调用的
    // 1.caller -> login(LoginRequest) -> muduo -> callee
    // 2.callee -> login(LoginRequest) -> 交到从下面重写的login方法上了
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done){

        // 框架给业务上报了请求参数 LoginRequest,业务获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 做本地业务
        bool login_result = Login(name,pwd); 

        // 把响应写入 包括错误码、错误消息、返回值  要是有异常得try catch
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        // 执行回调操作 执行响应对象数据的序列化和网络发送（都是由框架来完成的）
        done->Run();

    }

    // 重写Register虚函数 固定写法同上
    void Register(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done){
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool ret = Register(id,name,pwd);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(ret);

        done->Run();

    }


};


int main(int argc,char **argv){

    // 调用框架的初始化操作 provider -i config.conf
    MprpcApplication::Init(argc,argv);

    // provider是一个rpc网络服务对象，把userservice对象发布到rpc上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动一个rpc服务发布节点 Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;


}
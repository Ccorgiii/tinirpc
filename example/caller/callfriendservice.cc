#include <iostream>
#include "mprpcapplication.h"
#include "friend.pb.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

int main(int argc,char** argv){

    // 整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数（只初始化一次）
    MprpcApplication::Init(argc,argv);

    // 演示调用远程发布的rpc方法Login  rpc调用方法都在MprpcChannel中定义和重载，这里引用基类进行框架初始化
    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());// 这个stub是protobuf一定会生成吗?

    // rpc方法的请求参数
    fixbug::GetFriendsListRequest request;
    request.set_userid(1000);
 
    // rpc方法的响应
    fixbug::GetFriendsListResponse response;

    // 发起rpc方法的调用 同步的rpc调用过程 MprpcChannel:: method (同步阻塞的方式调用)

    MprpcController controller;

    //RpcChannel->RpcChannel::calMethod  集中来做所有rpc调用的参数序列化和网络发送
    //rpc调用端 control暂时用不到 done没有用
    stub.GetFriendsList(&controller,&request,&response,nullptr);

    // 一次rpc调用完成，读调用的结果
    if(controller.Failed()){

        std::cout << controller.ErrorText() <<std::endl;
    }
    else{

        if(0 == response.result().errcode()){

            std::cout<< "rpc login response success!"  <<std::endl;
            int size = response.friends_size();
            for(int i=0;i<size;++i){
                std::cout<<"index:" <<(i+1)<<"name"<<response.friends(i)<<std::endl;
        }
        }
        else{
            std::cout<< "rpc login response error:" << response.result().errmsg() <<std::endl;
        }

    }

    return 0;
}


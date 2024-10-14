#include "mprpcchannel.h"
#include "string"
#include "rpcheader.pb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <error.h>  
#include "mprpcapplication.h"
#include "mprpcController.h"
/*
header_size + service_name method_name args_size + args
*/
 
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller,
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response,
                          google::protobuf::Closure* done){

    const google::protobuf::ServiceDescriptor* sd =  method->service();
    std::string service_name = sd->name();// service_name
    std::string method_name = method->name();// method_name

    // 获取参数的序列化字符串长度 args_size
    int args_size = 0;
    std::string args_str;
    if(request->SerializeToString(&args_str)){
        args_size = args_str.size();

    }
    else{
        //std::cout<< "serialize request error!"<<std::endl;
        controller->SetFailed("serizalize request error!");
        return;

    }

    // 定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    int header_size = 0;
    std::string rpc_header_str;
    
    if(rpcHeader.SerializeToString(&rpc_header_str)){

        header_size = rpc_header_str.size();
    }
    else{
        //std::cout<< "serialize rpc header error!"<<std::endl;
        controller->SetFailed("serizalize request error!");
        return;
    }

    // 组织待发送的rpc请求字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0,std::string((char*)&header_size,4)); // header_size
    send_rpc_str += rpc_header_str; // rpcheader
    send_rpc_str += args_str;   // args

    // 打印调试信息
    std::cout <<"========================================="<<std::endl;
    std::cout<<"header_size:"<<header_size<<std::endl;
    std::cout<<"rpc_header_str:"<<rpc_header_str<<std::endl;
    std::cout<<"service_name:"<<service_name<<std::endl;
    std::cout<<"method_name:"<<method_name<<std::endl;
    std::cout<<"args_str:"<<args_str<<std::endl;
    std::cout <<"========================================="<<std::endl;

    // 使用原始TCP编程完成rpc调用的远程方法
    int clientfd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == clientfd){
        //std::cout << "creat socket error! errno:"<<errno<<std::endl;
        //exit(EXIT_FAILURE);
        char errtxt[512] = {0};
        sprintf(errtxt,"creat socket error! errno:%d",errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 读取配置文件rpcserver的信息
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务器节点
    if(-1 == connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr))){
        //std::cout << "connect error! errno:"<<errno<<std::endl;
        close(clientfd);
        //exit(EXIT_FAILURE); // 使用了controller的控制信息来替代直接关闭 在上层对这个controll进行判断
        char errtxt[512] = {0};
        sprintf(errtxt,"connect error! errno:%d",errno);
        controller->SetFailed(errtxt);
        return;
    }
   
   // 发送rpc请求
   if(-1 == send(clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0)){

        //std::cout << "send error! errno:"<<errno<<std::endl;
        char errtxt[512] = {0};
        sprintf(errtxt,"send error! errno:%d",errno);
        controller->SetFailed(errtxt);
        return;
   }
   
    // 接受rpc请求的响应值
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if(-1 == (recv_size = recv(clientfd,recv_buf,1024,0))){
        //std::cout << "recv error! errno:"<<errno<<std::endl;
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt,"recv error! errno:%d",errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 反序列化rpc调用的响应数据
    //std::string response_str(recv_buf,0,recv_size);// bug出现问题，recv_buf中遇到\0后面数据就存不下来，反序列化失败
    //if(!response->ParseFromString(response_str)){
    
    if(!response->ParseFromArray(recv_buf,recv_size)){
        //std::cout << "parse error! response_str:"<<recv_buf<<std::endl;
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt,"parse error! response_str:%s",recv_buf);
        controller->SetFailed(errtxt);
        return;
    }

    close(clientfd);

}


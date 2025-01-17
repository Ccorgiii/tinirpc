#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>

MprpcConfig MprpcApplication::m_config;// 静态成员初始化

void ShowArgsHelp(){
    std::cout<<"format: command -i <configgile>" <<std::endl;
}

void MprpcApplication::Init(int argc,char **argv){

    if(argc < 2){
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string congfig_file;
    while((c = getopt(argc,argv,"i:"))!=-1){
        switch(c){
            case 'i':
                congfig_file = optarg;//optarg是啥
                break;
            case '?':
                std::cout<<"invalid args!"<<std::endl;
                ShowArgsHelp();
                exit(EXIT_FAILURE);
               
            case ':':
                std::cout<<"need <configfile> "<<std::endl;
                ShowArgsHelp(); 
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    // 开始加载配置文件了 rpcserver_ip=  rpcserver_port=  zookeeper_ip= zookpper_port=
    m_config.LoadConfigFile(congfig_file.c_str());

    // std::cout << "rpcserverip:" <<m_config.Load("rpcserverip")<<std::endl;
    // std::cout << "rpcserverport:" <<m_config.Load("rpcserverport")<<std::endl;
    // std::cout << "zookeeperip:" <<m_config.Load("zookeeperip")<<std::endl;
    // std::cout << "zookeeperport:" <<m_config.Load("zookeeperport")<<std::endl;
}

MprpcApplication& MprpcApplication::GetInstance(){

    static MprpcApplication app;
    return app;

}

MprpcConfig& MprpcApplication::GetConfig(){
    return m_config;
}

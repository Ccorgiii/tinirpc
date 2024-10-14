#pragma once
#include "mprpcconfig.h"

// mprpc框架基础类
class MprpcApplication
{
public:
    static void Init(int argc,char **argv);
    static MprpcApplication& GetInstance();
    static MprpcConfig& GetConfig();

private:

    static MprpcConfig m_config;

    MprpcApplication(){}
    // 改为单例模式
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;    
};






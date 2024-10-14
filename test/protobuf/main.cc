#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main()
{
    // 封装了logfin请求的数据
    // LoginRequest rsp;
    // ResultCode *rc = rsp.mutable_result();
    // rc->set_errcode(1);
    // rc->set_errmsg("登陆失败了");


    // // 对象数据序列化 -> char*
    // std::string send_str;
    // if(req.SerializeToString(&send_str)){
    //     std::cout<< send_str << std::endl;
    // }
    
    // // 从send_str反序列化
    // LoginRequest reqB;
    // if(reqB.ParseFromString(send_str)){
    //     std::cout<< reqB.name() << std::endl;
    //     std::cout<< reqB.pwd() << std::endl;
    // }

    
    //LoginResponse

    GetFriendListsResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);
    
    User *user1 = rsp.add_friend_list();
    user1->set_name("zhang san");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    User *user2 = rsp.add_friend_list();
    user2->set_name("zhang san");
    user2->set_age(20);
    user2->set_sex(User::MAN);

    std::cout << rsp.friend_list_size() << std::endl;

    return 0;
}
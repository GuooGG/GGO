#include"GGo.h"

#define LOG GGO_LOG_DEBUG(GGO_LOG_ROOT())

/// @brief 测试socket模块与address模块的基本地址解析与信息收发功能
void test_socket(){
    std::cout << "socket test start" << std::endl;
    GGo::IPAddress::ptr address = GGo::Address::LookupAnyIPAddress("localhost");
    if(address){
        LOG << "address.ip=" << address->toString(); 
    }else{
        LOG << "get ip address failed";
    }

    GGo::Socket::ptr socket = GGo::Socket::CreateTCP(address);
    address->setPort(80);
    LOG << "address.ip=" << address->toString();
    if(!socket->connect(address)){
        GGO_LOG_ERROR(GGO_LOG_ROOT()) << "connect failed";
        return;
    }else{
        LOG << "connected to " << address->toString();
    }

    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = socket->send(buff, sizeof(buff));
    if(rt <= 0){
        GGO_LOG_ERROR(GGO_LOG_ROOT()) << "send message fail";
        return;
    }

    std::string buffer;
    buffer.resize(4096);
    rt = socket->recv(&buffer[0], buffer.size());

    if(rt <= 0){
        GGO_LOG_ERROR(GGO_LOG_ROOT()) << "receive message fail";
        return;
    }
    buffer.resize(rt);
    LOG << buffer;

    std::cout << "socket test end" << std::endl;
}


int main(){
    test_socket();
    
    return 0;
}
#include"address.h"
#include"logSystem.h"

#define LOG GGO_LOG_DEBUG(GGO_LOG_ROOT())

void test_basic(){
    LOG << "============ipv4 test begin===============";
    GGo::IPv4Address::ptr ipv4addr(GGo::IPv4Address::Create("192.168.10.2",80));
    LOG << ipv4addr->toString();
    LOG << ipv4addr->boradcastAdress(16)->toString();
    LOG << ipv4addr->networdAdress(16)->toString();
    LOG << ipv4addr->getAddrLen();
    LOG << "============ipv4 test end=================";
    LOG << "============ipv6 test begin===============";
    GGo::IPv6Address::ptr ipv6addr(GGo::IPv6Address::Create("fe80:fe80:fe80:fe80:1a67:f1f3:1b09:3be4", 80));
    LOG << ipv6addr->toString();
    LOG << ipv6addr->boradcastAdress(64)->toString();
    LOG << ipv6addr->networdAdress(64)->toString();
    LOG << ipv6addr->getAddrLen();
    LOG << "============ipv6 test end=================";
}


int main(){

    test_basic();

    return 0;
}
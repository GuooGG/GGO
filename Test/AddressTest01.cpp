#include"logSystem.h"
#include"config.h"
#include"address.h"
#include<vector>

#define LOG GGO_LOG_DEBUG(GGO_LOG_ROOT())
GGo::ConfigVar<std::string>::ptr g_hostname_config = GGo::Config::Lookup("dns_test.host", std::string("localhost"), "host for dns test");

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
    ipv6addr->setPort(88);
    LOG << ipv6addr->toString();
    LOG << "============ipv6 test end=================";
}

void test_dns(){
    std::vector<GGo::Address::ptr> addrs;
    bool v = GGo::Address::Lookup(addrs, g_hostname_config->getValue());
    if(!v){
        LOG << "lookup fail";
    }
    for(auto addr : addrs){
        LOG << addr->toString();
    }
}

int main(){
    YAML::Node cfg = YAML::LoadFile("/root/workspace/GGoSeverFrame/Test/conf/addr.yml");
    GGo::Config::loadFromYaml(cfg);
    // test_basic();
    test_dns();

    return 0;
}
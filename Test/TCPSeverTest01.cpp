#include "GGo.h"

void test_conf(){
    GGo::TCPSeverConf conf;
    conf.name = "test_sever";
    GGo::IPv4Address addr(114,80);
    conf.addresses.push_back("addr1");
    conf.addresses.push_back("addr2");
    conf.addresses.push_back(addr.toString());
    YAML::Node node = YAML::Load(GGo::LexicalCast<GGo::TCPSeverConf, std::string>()(conf));

    GGO_LOG_DEBUG(GGO_LOG_ROOT()) << std:: endl << node;
    GGO_LOG_DEBUG(GGO_LOG_ROOT()) << "==========================";
    std::stringstream ss;
    ss << node;
    std::string str = ss.str();
    GGO_LOG_DEBUG(GGO_LOG_ROOT())  << std::endl << str;
    GGO_LOG_DEBUG(GGO_LOG_ROOT()) << "==========================";
    GGo::TCPSeverConf conf2;
    conf2 = GGo::LexicalCast<std::string, GGo::TCPSeverConf>()(str);
    std::string str2;
    str2 = GGo::LexicalCast<GGo::TCPSeverConf, std::string>()(conf2);
    GGO_LOG_DEBUG(GGO_LOG_ROOT()) << std::endl << str2;


}



int main(){
    test_conf();
    return 0;
}
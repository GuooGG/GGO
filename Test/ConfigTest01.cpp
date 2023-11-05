#include"config.h"
#include"logSystem.h"
#include"bytearray.h"
int main(){
    GGo::ConfigVar<int>::ptr g_int_value_config = GGo::Config::Lookup("system.port" , (int)8080, "system port");
    GGO_LOG_INFO(GGO_LOG_ROOT()) << g_int_value_config->getValue();
    GGO_LOG_INFO(GGO_LOG_ROOT()) << g_int_value_config->toString();
    GGO_LOG_INFO(GGO_LOG_ROOT()) << g_int_value_config->getName();
    return 0;
}
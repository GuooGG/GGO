#include"GGo.h"

GGo::Logger::ptr g_Logger = GGO_LOG_NAME("system");
#define LOG GGO_LOG_DEBUG(g_Logger)

void test_basic(){
    sleep(100);
}

int main(){
    LOG << "main begin";
    test_basic();
    LOG << "main end";
    return 0;
}
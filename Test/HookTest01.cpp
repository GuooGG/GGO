#include"GGo.h"

GGo::Logger::ptr g_Logger = GGO_LOG_NAME("system");
#define LOG GGO_LOG_DEBUG(g_Logger)


void test_basic(){
    LOG << "test basic start";
    GGo::IOScheduler ioscheduler(2);
    ioscheduler.schedule([](){
        sleep(3);
        LOG << "sleep 3secs";
    });
    ioscheduler.schedule([](){
        sleep(2);
        LOG << "sleep 2secs";
    });
    LOG << "missons all scheduled";
    LOG << "test basic end";
}

int main(){
    LOG << "main begin";
    test_basic();
    LOG << "main end";
    return 0;
}
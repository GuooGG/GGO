#include"GGo.h"

GGo::Logger::ptr g_Logger = GGO_LOG_NAME("system");
#define LOG GGO_LOG_DEBUG(g_Logger)


void test_sleep(){
    LOG << "test sleep start";
    GGo::IOScheduler ioscheduler(1);
    ioscheduler.schedule([](){
        sleep(3);
        LOG << "sleep 3secs";
    });
    ioscheduler.schedule([](){
        sleep(2);
        LOG << "sleep 2secs";
    });
    LOG << "missons all scheduled";
    LOG << "test sleep end";
}

void test_usleep(){
    LOG << "test usleep start";
    GGo::IOScheduler ioscheduler(1);
    ioscheduler.schedule([](){
        usleep(1000000);
        LOG << "usleep 1000000us";
    });
    ioscheduler.schedule([](){
        usleep(1000000);
        LOG << "usleep 1000000us"; 
    });
    ioscheduler.schedule([]() {
        usleep(1000000);
        LOG << "usleep 1000000us"; 
    });
    LOG << "test usleep end";
    
}

void test_nanosleep(){
    LOG << "test nanosleep start";
    GGo::IOScheduler ioscheduler(1);
    struct timespec req,rem;
    req.tv_sec = 1;
    req.tv_nsec = 1000000000L;
    ioscheduler.schedule([&]() {
        nanosleep(&req, &rem);
        LOG << "nanosleep 2000ms";
    });
    ioscheduler.schedule([&]() {
        nanosleep(&req, &rem);
        LOG << "nanosleep 2000ms";
    });
    ioscheduler.schedule([&]() {
        nanosleep(&req, &rem);
        LOG << "nanosleep 2000ms";
    });
    LOG << "test nanosleep end";
}

int main(){
    LOG << "main begin";
    test_sleep();
    test_usleep();
    test_nanosleep();
    LOG << "main end";
    return 0;
}
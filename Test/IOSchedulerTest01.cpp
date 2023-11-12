#include"ioScheduler.h"
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<fcntl.h>
#include<logSystem.h>

GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

int sock = 0;
void test_fiber(){
    GGO_LOG_INFO(g_logger) << "test fiber begin";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "115.239.210.27", &addr.sin_addr.s_addr);

    if (!connect(sock, (const sockaddr *)&addr, sizeof(addr)))
    {
    }
    else if (errno == EINPROGRESS)
    {
        GGO_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
        GGo::IOScheduler::getThis()->addEvent(sock, GGo::IOScheduler::READ, []()
        { 
            GGO_LOG_INFO(g_logger) << "read callback"; 
        });
        GGo::IOScheduler::getThis()->addEvent(sock, GGo::IOScheduler::WRITE, []()
        {
            GGO_LOG_INFO(g_logger) << "write callback";
            //close(sock);
            // GGo::IOScheduler::getThis()->cancelEvent(sock, GGo::IOScheduler::READ);
            close(sock); 
        });
        GGO_LOG_INFO(g_logger) << "add event errno precess end";
    }
    else
    {
        GGO_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }

    GGO_LOG_INFO(g_logger) << "test fiber end";
}

void test_ioscheduler(){
    GGO_LOG_INFO(g_logger) << "test ioscheduler begin";

    GGo::IOScheduler scheduler(1,false,"test ioscheduler");
    scheduler.schedule(&test_fiber);

    GGO_LOG_INFO(g_logger) << "test ioscheduler end";
}
GGo::Timer::ptr s_timer;
void test_timer(){

    GGo::IOScheduler scheduler(1, false, "test timer");
    s_timer = scheduler.addTimer(1000,[](){
        static int i = 0;
        GGO_LOG_INFO(g_logger) << "hello, timer i= " << i;
        if(++i == 3){
            s_timer->reset(2000,false);
        }
    },false);
    GGO_LOG_DEBUG(g_logger) << "timer added";
}

int main(){
    GGO_LOG_INFO(g_logger) << "main begin";
    test_timer();
    GGO_LOG_INFO(g_logger) << "main end";

    return 0;
}
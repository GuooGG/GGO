#include"GGo.h"

GGo::Logger::ptr g_Logger = GGO_LOG_NAME("system");
#define LOG GGO_LOG_DEBUG(g_Logger)



// test sleep
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

// test socket
void test_socket_init(){
    int fdo = socket(1,1,0);
    int fdh;
    static GGo::IOScheduler iosc(2, false, "hook tester");
    iosc.schedule([&fdh](){
        fdh = socket(1,1,0);
        LOG << "hooked socket fd= " << fdh;
    });
    LOG << "original socket fd= " << fdo;

}

void test_socket_accept(){
    GGo::IPv4Address::ptr ipv4addr(new GGo::IPv4Address(0,0));
    int fd = socket(1,1,0);
    LOG << "ipv4addr= " << ipv4addr->toString();
    LOG << "socket fd= " << fd;
    int fda = accept(fd, nullptr, nullptr);
    LOG << "accepted socket fd= " << fda;
    static GGo::IOScheduler iosc(2, false, "hook tester");
    iosc.schedule([&fda,fd](){
        fda = accept(fd, nullptr, nullptr);
        LOG << "accepted socket fd= " << fda;
    });
}

void test_socket(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "192.168.23.128", &addr.sin_addr.s_addr);
    LOG << "connect begin";


    int rt = connect(sock, (const sockaddr*)&addr, sizeof(addr));
    LOG << "connect rt=" << rt << " errno=" << errno;

    if(rt){
        return;
    }
    LOG << "addr= " << addr.sin_addr.s_addr;
    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    LOG << "send rt=" << rt << " errno=" << errno;

    if(rt <= 0){
        return;
    }

    std::string buffer;
    buffer.resize(2129402);

    rt = recv(sock, &buffer[0], buffer.size(), 0);
    LOG << "recv rt=" << rt << " errno=" << errno;
    if(rt <= 0){
        return;
    }

    buffer.resize(rt);
    LOG << buffer;

}


int main(){
    LOG << "main begin";
    // test_sleep();
    // test_usleep();
    // test_nanosleep();
    // test_socket_init();
    // test_socket_accept();
    test_socket();
    // static GGo::IOScheduler iosc(1, false, "hook tester");
    // iosc.schedule(&test_socket);
    LOG << "main end";
    return 0;
}
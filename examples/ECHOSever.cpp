#include "TCPSever.h"
#include "logSystem.h"
#include "ioScheduler.h"
#include "bytearray.h"
#include "address.h"

static GGo::Logger::ptr g_logger = GGO_LOG_ROOT();

class EchoSever : public GGo::TCPSever{
public:
    EchoSever(int type);
    void handleCilent(GGo::Socket::ptr cilent) override;
private:
    int m_type;
};

EchoSever::EchoSever(int type)
    :m_type(type)
{

}

void EchoSever::handleCilent(GGo::Socket::ptr cilent)
{
    GGO_LOG_INFO(g_logger) << "new cilent connected" << cilent->toString();
    GGo::ByteArray::ptr ba(new GGo::ByteArray);
    while(true){
        ba->clear();
        std::vector<iovec> iovs;
        ba->getWriteBuffers(iovs, 1024);

        int rt = cilent->recv(&iovs[0], iovs.size());
        if(rt == 0){
            GGO_LOG_INFO(g_logger) << "cilent close: " << cilent->toString();
            break;
        }else if(rt < 0){
            GGO_LOG_INFO(g_logger) << "cilent error rt=" << rt
                                << " errno=" << errno << " errstr=" << strerror(errno);
            break;
        }
        ba->setPosition(ba->getPosition() + rt);
        ba->setPosition(0);
        if(m_type == 1){
            std::cout << ba->toString();
        }else{
            std::cout << ba->toHexString();
        }
        std::cout.flush();
    }

}

int type = 1;

void run_echo_sever(){
    GGO_LOG_INFO(g_logger) << "sever type=" << type;
    EchoSever::ptr es(new EchoSever(type));
    auto addr = GGo::Address::LookupAny("localhost:1145");
    while(!es->bind(addr)){
        sleep(1);
    }
    es->start();
}

int main(int argc, char** argv){
    if(argc < 2){
        GGO_LOG_INFO(g_logger) << "used as[" << argv[0] << " -t] or [" << argv[0] << " -b]";
        return 0;
    }
    if(!strcmp(argv[1], "-b")){
        type = 2;
    }

    GGo::IOScheduler ios(2);
    ios.schedule(run_echo_sever);

    return 0;
}
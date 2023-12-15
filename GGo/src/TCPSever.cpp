#include "TCPSever.h"
#include "config.h"
#include "logSystem.h"

namespace GGo{

static GGo::ConfigVar<uint64_t>::ptr g_tcp_sever_read_timeout = 
    GGo::Config::Lookup("tcp_sever.read_timeout", (uint64_t)(60 * 1000 * 2),
            "tcp sever read timeout");

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

void TCPSever::setConf(const TCPSeverConf &conf)
{
    m_conf.reset(new TCPSeverConf(conf));
}

std::string TCPSever::toString(const std::string &prefix)
{
    std::stringstream ss;
    ss << prefix << "[type=" << m_type
        << " name=" << m_name
        << " ssl=" << m_ssl
        << " worker=" << (m_worker ? m_worker->getName() : "")
        << " accept=" << (m_acceptWorker ? m_acceptWorker->getName() : "")
        << " recv_timeout=" << m_recvTimeout << "]" << std::endl;
    std::string pre = prefix.empty() ? "    " : prefix;
    for(auto& sock : m_socks){
        ss << pre << pre << *sock << std::endl;
    }
    return ss.str();
}

void TCPSever::handleCilent(Socket::ptr cilent)
{
    GGO_LOG_INFO(g_logger) << "handleCilent: " << cilent->toString();
}

void TCPSever::startAccept(Socket::ptr sock)
{
    while (!m_isStop)
    {
        Socket::ptr cilent = sock->accept();
        if(cilent){
            cilent->setRecvTimeout(m_recvTimeout);
            m_ioWorker->schedule(std::bind(&TCPSever::handleCilent, shared_from_this(), cilent));
        }else{
            GGO_LOG_ERROR(g_logger) << "accept errno=" << errno << " errstr=" << strerror(errno);
        }
    }
}

TCPSever::TCPSever(GGo::IOScheduler *worker, GGo::IOScheduler *io_worker, GGo::IOScheduler *accept_worker)
    :m_worker(worker)
    ,m_ioWorker(io_worker)
    ,m_acceptWorker(accept_worker)
    ,m_recvTimeout(g_tcp_sever_read_timeout->getValue())
    ,m_name("GGO/1.0.0")
    ,m_isStop(true)
{
}

TCPSever::~TCPSever()
{
    for(auto& sock : m_socks){
        sock->close();
    }
    m_socks.clear();
}
bool TCPSever::bind(GGo::Address::ptr addr, bool ssl)
{
    std::vector<Address::ptr> addrs;
    std::vector<Address::ptr> fails;
    addrs.push_back(addr);
    return this->bind(addrs, fails, ssl);
}
bool TCPSever::bind(const std::vector<Address::ptr> &addrs, std::vector<Address::ptr> &fails, bool ssl)
{
    m_ssl = ssl;
    for(auto& addr : addrs){
        Socket::ptr sock = Socket::CreateTCP(addr);
        if(!sock->bind(addr)){
            GGO_LOG_ERROR(g_logger) << "bind fail errno=" << errno
                                << " errstr=" << strerror(errno)
                                << " addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            continue;
        }
        if(!sock->listen()){
            GGO_LOG_ERROR(g_logger) << "listen fail errno=" << errno
                                << " errstr=" << strerror(errno)
                                << " addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            continue;
        }
        m_socks.push_back(sock);
        
    }
    if(!fails.empty()){
            m_socks.clear();
            return false;
    }
    for(auto& success_sock : m_socks){
        GGO_LOG_INFO(g_logger) << std::endl << "type=" << m_type
                            << " name=" << m_name 
                            << " ssl=" << m_ssl
                            << " sever bind seccess: " << success_sock->toString();
    }

    return true;
}
bool TCPSever::start()
{
    if(!m_isStop){
        return true;
    }
    m_isStop = false;
    for(auto& sock : m_socks){
        m_acceptWorker->schedule(std::bind(&TCPSever::startAccept, shared_from_this(), sock));
    }
    return true;
}
void TCPSever::stop()
{
    //TODO:: 这里的m_isStop要加锁吗
    m_isStop = true;
    auto self = shared_from_this();
    m_acceptWorker->schedule([this, self](){
        for(auto& sock : m_socks){
            sock->cancelAll();
            sock->close();
        }
        m_socks.clear();
    });
}
}

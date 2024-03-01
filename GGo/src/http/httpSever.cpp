#include "http/httpSever.h"
#include "logSystem.h"
#include "httpSever.h"
#include "http/servlets/configServlet.h"
#include "http/servlets/statusServlet.h"
namespace GGo{
namespace HTTP{

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

HTTPSever::HTTPSever(bool keepalive, 
                    GGo::IOScheduler *worker, 
                    GGo::IOScheduler *io_worker, 
                    GGo::IOScheduler *accept_worker)
        :TCPSever(worker, io_worker, accept_worker)
        ,m_isKeepAlive(keepalive)
{
    m_dispatch.reset(new ServletDispatch);

    m_type = "http";
     m_dispatch->addServlet("/_/config", servlet::ptr(new ConfigServlet));
     m_dispatch->addServlet("/_/status", servlet::ptr(new statusServlet));
}

void HTTPSever::setName(const std::string &name)
{
    TCPSever::setName(name);
    m_dispatch->setDefault(std::make_shared<NotFoundServlet>(name));
}

void HTTPSever::handleCilent(Socket::ptr cilent)
{
    GGO_LOG_DEBUG(g_logger) << "handlerCilent: " << *cilent;
    HTTPSession::ptr session(new HTTPSession(cilent));
    do{
        auto req = session->recvRequest();
        if(!req){
            GGO_LOG_DEBUG(g_logger) << "recv http request fail, errno="
                    << errno << " errstr=" << strerror(errno) << " cilent:"
                    << *cilent << " keepalive=" << m_isKeepAlive;
            break;
        }

        HTTPResponse::ptr rsp(new HTTPResponse(req->getVersion(),
                                                req->isAutoClose() || !m_isKeepAlive));
        rsp->setHeader("Sever", getName());
        m_dispatch->handle(req,rsp, session);
        session->sendResponse(rsp);

        if(!m_isKeepAlive || req->isAutoClose()){
            break;
        }
    }while(true);
    session->close();
}
}
}



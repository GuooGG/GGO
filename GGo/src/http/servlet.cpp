#include "servlet.h"
#include<fnmatch.h>

namespace GGo{
namespace HTTP{

ServletDispatch::ServletDispatch()
    :servlet("ServletDispatch")
{
    m_default.reset(new NotFoundServlet("GGo/1.0"));
}

int32_t ServletDispatch::handle(GGo::HTTP::HTTPRequest::ptr request, GGo::HTTP::HTTPResponse::ptr response, GGo::HTTP::HTTPSession::ptr session)
{
    auto slt = getMatchedServlet(request->getPath());
    if(slt){
        slt->handle(request, response, session);
    }
    return 0;
}

void ServletDispatch::addServlet(const std::string &uri, servlet::ptr slt)
{
    RWMutexType::writeLock lock(m_mutex);
    m_datas[uri] = std::make_shared<HoldServletCreator>(slt);
}

void ServletDispatch::addServlet(const std::string &uri, functionServlet::callback cb)
{
    RWMutexType::writeLock lock(m_mutex);
    m_datas[uri] = std::make_shared<HoldServletCreator>(std::make_shared<functionServlet>(cb));
}

void ServletDispatch::addGlobServlet(const std::string &uri, servlet::ptr slt)
{
    RWMutexType::writeLock lock(m_mutex);
    for(auto it = m_globs.begin(); it != m_globs.end(); it++){
        if(it->first == uri){
            m_globs.erase(it);
            break;
        }
    }
    m_globs.push_back(std::make_pair(uri, std::make_shared<HoldServletCreator>(slt)));
}

void ServletDispatch::addGlobServlet(const std::string &uri, functionServlet::callback cb)
{
    return addGlobServlet(uri, std::make_shared<functionServlet>(cb));
}

void ServletDispatch::addServletCreator(const std::string &uri, IServeltCreator::ptr creator)
{
    RWMutexType::writeLock lock(m_mutex);
    m_datas[uri] = creator;
}

void ServletDispatch::addGlobServletCreator(const std::string &uri, IServeltCreator::ptr creator)
{
    RWMutexType::writeLock lock(m_mutex);
    for(auto it = m_globs.begin(); it != m_globs.end(); it++){
        if(it->first == uri){
            m_globs.erase(it);
            break;
        }
    }
    m_globs.push_back(std::make_pair(uri, creator));
}

void ServletDispatch::delServlet(const std::string &uri)
{
    RWMutexType::writeLock lock(m_mutex);
    m_datas.erase(uri);
}

void ServletDispatch::delGlobServlet(const std::string &uri)
{
    RWMutexType::writeLock lock(m_mutex);
    for(auto it = m_globs.begin(); it != m_globs.end(); it++){
        if(it->first == uri){
            m_globs.erase(it);
            break;
        }
    }
}

servlet::ptr ServletDispatch::getServlet(const std::string &uri)
{
    RWMutexType::readLock lock(m_mutex);
    auto it = m_datas.find(uri);
    return it == m_datas.end() ? nullptr : it->second->get();
}

servlet::ptr ServletDispatch::getGlobServlet(const std::string &uri)
{
    RWMutexType::readLock lock(m_mutex);
    for(auto it = m_globs.begin();
            it != m_globs.end(); ++it) {
        if(it->first == uri) {
            return it->second->get();
        }
    }
    return nullptr;
}

servlet::ptr ServletDispatch::getMatchedServlet(const std::string &uri)
{
    RWMutexType::readLock lock(m_mutex);
    auto mit = m_datas.find(uri);
    if(mit != m_datas.end()){
        return mit->second->get();
    }
    for(auto it = m_globs.begin();
            it != m_globs.end(); ++it) {
        if(!fnmatch(it->first.c_str(), uri.c_str(), 0)) {
            return it->second->get();
        }
    }
    return m_default;

}

void ServletDispatch::listAllServletCreator(std::map<std::string, IServeltCreator::ptr> &infos)
{
    RWMutexType::readLock lock(m_mutex);
    for(auto& i : m_datas) {
        infos[i.first] = i.second;
    }
}

void ServletDispatch::listAllGlobServletCerator(std::map<std::string, IServeltCreator::ptr> &infos)
{
    RWMutexType::readLock lock(m_mutex);
    for(auto& i : m_globs) {
        infos[i.first] = i.second;
    }
}

NotFoundServlet::NotFoundServlet(const std::string &name)
    :servlet("NotFoundServlet")
    ,m_name(name)
{
    m_content = "<html><head><title>404 Not Found"
        "</title></head><body><center><h1>404 Not Found</h1></center>"
        "<hr><center>" + name + "</center></body></html>";
}

int32_t NotFoundServlet::handle(HTTPRequest::ptr request
                                , HTTPResponse::ptr response
                                , HTTPSession::ptr session)
{
    response->setStatus(GGo::HTTP::HTTPStatus::NOT_FOUND);
    response->setHeader("Server", "GGo.1.0.0");
    response->setHeader("Content-Type", "text/html");
    response->setBody(m_content);
    return 0;
}

functionServlet::functionServlet(callback cb)
        :servlet("FunctionServlet")
        ,m_callback(cb)
{
}

int32_t functionServlet::handle(GGo::HTTP::HTTPRequest::ptr request, GGo::HTTP::HTTPResponse::ptr response, GGo::HTTP::HTTPSession::ptr session)
{
    return m_callback(request, response, session);
}
}
}

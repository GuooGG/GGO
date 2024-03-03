#include "http/httpConnection.h"
#include "httpConnection.h"


namespace GGo{ 
namespace HTTP{


std::string HTTPResult::toString() const
{
    std::stringstream ss;
    ss << "[HTTPResult result=" << result
        << " error=" << error
        << " response=" << (response ? response->toString() : "nullptr")
        << "]";
    return ss.str();
}

HTTPResult::ptr HTTPConnection::sendRequest(HTTPMethod method, const std::string &url, uint64_t timeout_ms, const std::map<std::string, std::string> &headers, const std::string &body)
{
    Uri::ptr uri = Uri::Create(url);
    if(!uri){
        return std::make_shared<HTTPResult>((int)HTTPResult::Error::INVALID_URL
                                            , nullptr
                                            , "invalid url: " + url);
    }
    return sendRequest(method, uri, timeout_ms, headers, body);
}

HTTPResult::ptr HTTPConnection::sendRequest(HTTPMethod method, Uri::ptr url, uint64_t timeout_ms, const std::map<std::string, std::string> &headers, const std::string &body)
{
    HTTPRequest::ptr request = std::make_shared<HTTPRequest>();
    request->setParam(url->getPath());
    request->setQuery(url->getQuery());
    request->setFrgment(url->getFragment());
    request->setMethod(method);
    bool has_host = false;
    for(auto& i : headers){
        if(strcasecmp(i.first.c_str(), "connection") == 0){
            if(strcasecmp(i.second.c_str(), "keep-alive") == 0){
                request->setAutoClose(false);
            }
            continue;
        }
        if(has_host == false && strcasecmp(i.first.c_str(), "host") == 0){
            has_host = !i.second.empty();
        }
        request->setHeader(i.first,i.second);
    }
    request->setBody(body);
    return sendRequest(request, url, timeout_ms);
}

HTTPResult::ptr HTTPConnection::sendRequest(HTTPRequest::ptr request, Uri::ptr url, uint64_t timeout)
{
    bool is_ssl = url->getScheme() == "https";
    Address::ptr addr = url->createAddress();
    if(!addr){
        return std::make_shared<HTTPResult>((int)HTTPResult::Error::INVALID_HOST
                                            , nullptr
                                            , "invalid host: " + url->getHost());
    }
    Socket::ptr sock = Socket::CreateTCP(addr);
    if(!sock){
        return std::make_shared<HTTPResult>((int)HTTPResult::Error::CREATE_SOCKET_ERROR
                                            , nullptr
                                            , "create socket fail: " + addr->toString()
                                            + "errno=" + std::to_string(errno
                                            + "errstr=" + std::string(strerror(errno))));
    }
    //TODO::this
}

HTTPConnection::HTTPConnection(Socket::ptr sock, bool owner)
    : SocketStream(sock, owner)
{
}

HTTPConnection::~HTTPConnection()
{

}
int HTTPConnection::sendRequest(HTTPRequest::ptr request)
{
    std::string data = request->toString();
    return writeFixSize(data.c_str(), data.size());
}
}
}


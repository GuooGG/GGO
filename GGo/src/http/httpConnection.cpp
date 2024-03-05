#include "http/httpConnection.h"
#include "httpConnection.h"
#include"http/httpParser.h"
#include "logSystem.h"

namespace GGo{ 
namespace HTTP{

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

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
    request->setPath(url->getPath());
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
    //bool is_ssl = url->getScheme() == "https";
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
                                            + "errno=" + std::to_string(errno)
                                            + "errstr=" + std::string(strerror(errno)));
    }
    if(!sock->connect(addr)){
        return std::make_shared<HTTPResult>((int)HTTPResult::Error::CONNECTION_FAIL
                                            , nullptr
                                            , "connect fail: " + addr->toString());
    }
    sock->setRecvTimeout(timeout);
    HTTPConnection::ptr connect = std::make_shared<HTTPConnection>(sock);
    int rt = connect->sendRequest(request);
    if(rt == 0){
        return std::make_shared<HTTPResult>((int)HTTPResult::Error::SEND_CLOSE_BY_PEER
                                            , nullptr
                                            , "send close by peer: " + addr->toString());
    }
    if(rt < 0){
        return std::make_shared<HTTPResult>((int)HTTPResult::Error::TIMEOUT
                                            , nullptr
                                            , "send request socket error errno=" + std::to_string(errno)
                                             + " errstr=" + std::string(strerror(errno)));
    }
    auto response = connect->recvResponse();
    if(!response){
        return std::make_shared<HTTPResult>((int)HTTPResult::Error::TIMEOUT
                                            , nullptr
                                            , "recv response timeout: " + addr->toString()
                                            + " timeout_ms:" + std::to_string(timeout));
    }
    return std::make_shared<HTTPResult>((int)HTTPResult::Error::OK, response, "ok");
}


HTTPConnection::HTTPConnection(Socket::ptr sock, bool owner)
    : SocketStream(sock, owner)
{
}

HTTPConnection::~HTTPConnection()
{

}
HTTPResponse::ptr HTTPConnection::recvResponse()
{
    HTTPResponseParser::ptr parser(new HTTPResponseParser);
    uint64_t buffer_size = HTTPResponseParser::getHTTPResponseBufferSize();
    std::shared_ptr<char> buffer(
        new char[buffer_size + 1], [](char* ptr){
            delete[] ptr;
        }
    );
    char* data = buffer.get();
    int offset = 0;
    do{
        int len = read(data + offset, buffer_size - offset);
        if(len <= 0){
            close();
            return nullptr;
        }
        len += offset;
        data[len] = '\0';
        size_t nparse = parser->excute(data, len, false);
        if(parser->hasError()){
            close();
            return nullptr;
        }
        offset = len - nparse;
        if(offset == (int)buffer_size){
            close();
            return nullptr;
        }
        if(parser->isFinished()){
            break;
        }
    }while(true);
    
    auto& client_parser = parser->getParser();
    std::string body;
if(client_parser.chunked) {
        int len = offset;
        do {
            bool begin = true;
            do {
                if(!begin || len == 0) {
                    int rt = read(data + len, buffer_size - len);
                    if(rt <= 0) {
                        close();
                        return nullptr;
                    }
                    len += rt;
                }
                data[len] = '\0';
                size_t nparse = parser->excute(data, len, true);
                if(parser->hasError()) {
                    close();
                    return nullptr;
                }
                len -= nparse;
                if(len == (int)buffer_size) {
                    close();
                    return nullptr;
                }
                begin = false;
            } while(!parser->isFinished());
            //len -= 2;
            
            GGO_LOG_DEBUG(g_logger) << "content_len=" << client_parser.content_len;
            if(client_parser.content_len + 2 <= len) {
                body.append(data, client_parser.content_len);
                memmove(data, data + client_parser.content_len + 2
                        , len - client_parser.content_len - 2);
                len -= client_parser.content_len + 2;
            } else {
                body.append(data, len);
                int left = client_parser.content_len - len + 2;
                while(left > 0) {
                    int rt = read(data, left > (int)buffer_size ? (int)buffer_size : left);
                    if(rt <= 0) {
                        close();
                        return nullptr;
                    }
                    body.append(data, rt);
                    left -= rt;
                }
                body.resize(body.size() - 2);
                len = 0;
            }
        } while(!client_parser.chunks_done);
    } else {
        int64_t length = parser->getContentLength();
        if(length > 0) {
            body.resize(length);

            int len = 0;
            if(length >= offset) {
                memcpy(&body[0], data, offset);
                len = offset;
            } else {
                memcpy(&body[0], data, length);
                len = length;
            }
            length -= offset;
            if(length > 0) {
                if(readFixSize(&body[len], length) <= 0) {
                    close();
                    return nullptr;
                }
            }
        }
    }
    if(!body.empty()) {
        auto content_encoding = parser->getData()->getHeader("content-encoding");
        GGO_LOG_DEBUG(g_logger) << "content_encoding: " << content_encoding
            << " size=" << body.size();
        if(strcasecmp(content_encoding.c_str(), "gzip") == 0) {
            auto zs = ZlibStream::CreateGzip(false);
            zs->write(body.c_str(), body.size());
            zs->flush();
            zs->getResult().swap(body);
        } else if(strcasecmp(content_encoding.c_str(), "deflate") == 0) {
            auto zs = ZlibStream::CreateDeflate(false);
            zs->write(body.c_str(), body.size());
            zs->flush();
            zs->getResult().swap(body);
        }
        parser->getData()->setBody(body);
    }
    return parser->getData();


    return nullptr;
}
int HTTPConnection::sendRequest(HTTPRequest::ptr request)
{
    std::string data = request->toString();
    return writeFixSize(data.c_str(), data.size());
}
}
}


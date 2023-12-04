#include "http.h"

namespace GGo{
namespace HTTP{

HTTPMethod StringToHTTPMethod(const std::string &str)
{
#define XX(num, name, string) \
    if(strcmp(#string, str.c_str()) == 0){ \
        return HTTPMethod::name; \
    }
    HTTP_METHOD_MAP(XX);
#undef XX
    return HTTPMethod::INVALID_METHOD;
}

HTTPMethod StringToHTTPMethod(const char *str)
{
#define XX(num, name, string)                        \
    if (strncmp(#string, str, strlen(#string)) == 0) \
    {                                              \
        return HTTPMethod::name;                   \
    }
    HTTP_METHOD_MAP(XX);
#undef XX
    return HTTPMethod::INVALID_METHOD;
}

static const char* s_method_string[] = {
#define XX(num, name, string) #string,
    HTTP_METHOD_MAP(XX)
#undef XX
};

const char * HTTPMethodToString(const HTTPMethod &method)
{
    uint32_t index = (uint32_t)method;
    if((index >= sizeof(s_method_string) / sizeof(s_method_string[0]))){
        return "<unknown>";
    }
    return s_method_string[index];
}

const char *HTTPStatusToString(const HTTPStatus &status)
{
    switch(status) {
#define XX(code, name, msg) \
        case HTTPStatus::name: \
            return #msg;
        HTTP_STATUS_MAP(XX);
#undef XX
        default:
            return "<unknown>";
    }
}

bool StringComparator::operator()(const std::string &lhs, const std::string &rhs) const
{
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

HTTPRequest::HTTPRequest(uint8_t version, bool auto_close)
    :m_method(HTTPMethod::GET)
    ,m_version(version)
    ,m_autoClose(auto_close)
    ,m_isWebsocket(false)
{
}

std::string HTTPRequest::getHeader(const std::string &key, const std::string &def) const
{
    auto it = m_headers.find(key);
    return it == m_headers.end()? def : it->second;
}

std::ostream &HTTPRequest::dump(std::ostream &os) const
{
    os << HTTPMethodToString(m_method) << " "
        << m_path
        << (m_query.empty() ? "" : "?")
        << m_query
        << (m_fragment.empty() ? "" : "#")
        << m_fragment;
    os << " HTTP/" << ((uint32_t)(m_version >> 4))
        << "."
        << ((uint32_t)(m_version & 0x0F))
        << "\r\n";
    if(!m_isWebsocket){
        os << "connection: " << (m_autoClose ? "close" : "keep_alive") << "\r\n";
    }
    
    for(auto& item : m_headers){
        if(!m_isWebsocket && strcasecmp(item.first.c_str(), "connection") == 0){
            continue;
        }
        os << item.first << ": " << item.second << "\r\n";
    }
    
    if(!m_body.empty()){
        os << "connection-length: " << m_body.size() << "\r\n\r\n"
            << m_body;
    }else{
        os << "\r\n";
    }
    return os;

}

std::string HTTPRequest::toString() const
{
    std::stringstream ss;
    dump(ss);
    return ss.str();
}
void HTTPRequest::init()
{
    std::string connection = getHeader("connection");
    if(!connection.empty()){
        if(strcasecmp(connection.c_str(), "keep-alive") == 0){
            m_autoClose = false;
        }else{
            m_autoClose = true;
        }
    }
}
void HTTPRequest::initParam()
{
    initQueryParam();
    initBodyParam();
    initCookies();
}
void HTTPRequest::initQueryParam()
{
    if(m_parserParamFlag & 0x1){
        return;
    }

    size_t pos = 0;
    do
    {
        pos = m_query.find('=', pos);
        if(pos == std::string::npos){
            break;
        }
        size_t key_pos = pos;
        pos = m_query.find('&', pos);

    } while (true);
    

}
void HTTPRequest::initBodyParam()
{
    if(m_parserParamFlag& 0x2){
        return;
    }
}
void HTTPRequest::initCookies()
{
    if(m_parserParamFlag & 0x4){
        return;
    }
}
}
}






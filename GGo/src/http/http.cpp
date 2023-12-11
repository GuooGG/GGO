#include "http.h"
#include "util.h"

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

std::ostream &operator<<(std::ostream &os, const HTTPRequest &request)
{
    return request.dump(os);
}

std::ostream &operator<<(std::ostream &os, const HTTPResponse &response)
{
    return response.dump(os);
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

std::shared_ptr<HTTPResponse> HTTPRequest::createResponse()
{
    HTTPResponse::ptr response(new HTTPResponse(getVersion(), isAutoClose()));
    return response;
}

std::string HTTPRequest::getHeader(const std::string &key, const std::string &def) const
{
    auto it = m_headers.find(key);
    return it == m_headers.end()? def : it->second;
}

std::string HTTPRequest::getParam(const std::string &key, const std::string &def)
{
    initQueryParam();
    initBodyParam();
    auto it = m_params.find(key);
    return it == m_params.end() ? def : it->second;
}

std::string HTTPRequest::getCookies(const std::string &key, const std::string &def)
{
    initCookies();
    auto it = m_cookies.find(key);
    return it == m_cookies.end() ? def : it->second;
}

void HTTPRequest::setHeader(const std::string &key, const std::string &value)
{
    m_headers[key] = value;
}

void HTTPRequest::setParam(const std::string &key, const std::string &value)
{
    m_params[key] = value;
}

void HTTPRequest::setCookies(const std::string &key, const std::string &value)
{
    m_cookies[key] = value;
}

void HTTPRequest::delHeader(const std::string &key)
{
    m_headers.erase(key);
}

void HTTPRequest::delParam(const std::string &key)
{
    m_params.erase(key);
}

void HTTPRequest::delCookies(const std::string &key)
{
    m_cookies.erase(key);
}

bool HTTPRequest::hasHeader(const std::string &key, std::string *val)
{
    auto it = m_headers.find(key);
    if(it == m_headers.end()){
        return false;
    }
    if(val){
        *val = it->second;
    }
    return true;
}

bool HTTPRequest::hasParam(const std::string &key, std::string *val)
{
    initQueryParam();
    initBodyParam();
    auto it = m_params.find(key);
    if(it == m_params.end()){
        return false;
    }
    if(val){
        *val = it->second;
    }
    return true;
}

bool HTTPRequest::hasCookies(const std::string &key, std::string *val)
{
    initCookies();
    auto it = m_cookies.find(key);
    if(it == m_cookies.end()){
        return false;
    }
    if(val){
        *val = it->second;
    }
    return true;
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
        size_t last = pos;
        pos = m_query.find('=', pos);
        if(pos == std::string::npos){
            break;
        }
        size_t key_pos = pos;
        pos = m_query.find('&', pos);

        m_params.insert(std::make_pair(m_query.substr(last, key_pos - last), GGo::StringUtil::urlDecode(m_body.substr(key_pos + 1, pos - key_pos - 1))));
        if(pos == std::string::npos){
            break;
        }
        pos++;
    } while (true);
    
    m_parserParamFlag |= 0x1;
    return;
}
void HTTPRequest::initBodyParam()
{
    if(m_parserParamFlag& 0x2){
        return;
    }

    std::string content_type = getHeader("content-type");
    if(strcasestr(content_type.c_str(), "application/x-www-form-urlencode") == nullptr){
        m_parserParamFlag |= 0x2;
        return;
    }
    //TODO:: fix this
    size_t pos = 0;
    do
    {
        size_t last = pos;
        pos = m_query.find('=', pos);
        if(pos == std::string::npos){
            break;
        }
        size_t key_pos = pos;
        pos = m_query.find('&', pos);

        m_params.insert(std::make_pair(m_query.substr(last, key_pos - last), GGo::StringUtil::urlDecode(m_body.substr(key_pos + 1, pos - key_pos - 1))));
        if(pos == std::string::npos){
            break;
        }
        pos++;
    } while (true);
    
    m_parserParamFlag |= 0x2;
    return;

}
void HTTPRequest::initCookies()
{
    if(m_parserParamFlag & 0x4){
        return;
    }

    //TODO:: fix this
    size_t pos = 0;
    do
    {
        size_t last = pos;
        pos = m_query.find('=', pos);
        if(pos == std::string::npos){
            break;
        }
        size_t key_pos = pos;
        pos = m_query.find('&', pos);

        m_params.insert(std::make_pair(m_query.substr(last, key_pos - last), GGo::StringUtil::urlDecode(m_body.substr(key_pos + 1, pos - key_pos - 1))));
        if(pos == std::string::npos){
            break;
        }
        pos++;
    } while (true);

}
HTTPResponse::HTTPResponse(uint8_t version, bool auto_close)
    :m_status(HTTPStatus::OK)
    ,m_version(version)
    ,m_autoClose(auto_close)
    ,m_isWebsocket(false)
{
    
}
std::ostream &HTTPResponse::dump(std::ostream &os) const
{
    os << "HTTP/"
        << ((uint32_t)(m_version >> 4))
        << "."
        << ((uint32_t)(m_version & 0X0F))
        << " "
        << ((uint32_t)m_status)
        << " "
        << (m_reasons.empty() ? HTTPStatusToString(m_status) : m_reasons)
        << "\r\n";
    
    for(auto header_item : m_headers){
        if(!m_isWebsocket && strcasecmp(header_item.first.c_str(), "connection") == 0){
            continue;
        }
        os << header_item.first << ": " << header_item.second << "\r\n";
    }

    for(auto cookie_item : m_cookies){
        os << "Set-Cookie: " << cookie_item << "\r\n";
    }

    if(!m_isWebsocket){
        os << "connection: " << (m_autoClose ? "close" : "keep-alive") << "\r\n";
    }

    if(!m_body.empty()){
        os << "content-length: " << m_body.size() << "\r\n\r\n";
        os << m_body;
    }else{
        os << "\r\n";
    }
    
    return os;
}
std::string HTTPResponse::toString() const
{
    std::stringstream ss;
    dump(ss);
    return ss.str();
}
}
}






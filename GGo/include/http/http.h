/**
 * @file http.h
 * @author GGO
 * @brief HTTP协议封装
 * @date 2023-12-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace GGo{
namespace HTTP{

/* Request Methods */
#define HTTP_METHOD_MAP(XX)           \
    XX(0,  DELETE,      DELETE)       \
    XX(1,  GET,         GET)          \
    XX(2,  HEAD,        HEAD)         \
    XX(3,  POST,        POST)         \
    XX(4,  PUT,         PUT)          \
    /* pathological */                \
    XX(5,  CONNECT,     CONNECT)      \
    XX(6,  OPTIONS,     OPTIONS)      \
    XX(7,  TRACE,       TRACE)        \
    /* WebDAV */                      \
    XX(8,  COPY,        COPY)         \
    XX(9,  LOCK,        LOCK)         \
    XX(10, MKCOL,       MKCOL)        \
    XX(11, MOVE,        MOVE)         \
    XX(12, PROPFIND,    PROPFIND)     \
    XX(13, PROPPATCH,   PROPPATCH)    \
    XX(14, SEARCH,      SEARCH)       \
    XX(15, UNLOCK,      UNLOCK)       \
    XX(16, BIND,        BIND)         \
    XX(17, REBIND,      REBIND)       \
    XX(18, UNBIND,      UNBIND)       \
    XX(19, ACL,         ACL)          \
    /* subversion */                  \
    XX(20, REPORT,      REPORT)       \
    XX(21, MKACTIVITY,  MKACTIVITY)   \
    XX(22, CHECKOUT,    CHECKOUT)     \
    XX(23, MERGE,       MERGE)        \
    /* upnp */                        \
    XX(24, MSEARCH,     M-SEARCH)     \
    XX(25, NOTIFY,      NOTIFY)       \
    XX(26, SUBSCRIBE,   SUBSCRIBE)    \
    XX(27, UNSUBSCRIBE, UNSUBSCRIBE)  \
    /* RFC-5789 */                    \
    XX(28, PATCH,       PATCH)        \
    XX(29, PURGE,       PURGE)        \
    /* CalDAV */                      \
    XX(30, MKCALENDAR,  MKCALENDAR)   \
    /* RFC-2068, section 19.6.1.2 */  \
    XX(31, LINK,        LINK)         \
    XX(32, UNLINK,      UNLINK)       \
    /* icecast */                     \
    XX(33, SOURCE,      SOURCE)       \

/* Status Codes */
#define HTTP_STATUS_MAP(XX)                                                   \
    XX(100, CONTINUE,                        Continue)                        \
    XX(101, SWITCHING_PROTOCOLS,             Switching Protocols)             \
    XX(102, PROCESSING,                      Processing)                      \
    XX(200, OK,                              OK)                              \
    XX(201, CREATED,                         Created)                         \
    XX(202, ACCEPTED,                        Accepted)                        \
    XX(203, NON_AUTHORITATIVE_INFORMATION,   Non-Authoritative Information)   \
    XX(204, NO_CONTENT,                      No Content)                      \
    XX(205, RESET_CONTENT,                   Reset Content)                   \
    XX(206, PARTIAL_CONTENT,                 Partial Content)                 \
    XX(207, MULTI_STATUS,                    Multi-Status)                    \
    XX(208, ALREADY_REPORTED,                Already Reported)                \
    XX(226, IM_USED,                         IM Used)                         \
    XX(300, MULTIPLE_CHOICES,                Multiple Choices)                \
    XX(301, MOVED_PERMANENTLY,               Moved Permanently)               \
    XX(302, FOUND,                           Found)                           \
    XX(303, SEE_OTHER,                       See Other)                       \
    XX(304, NOT_MODIFIED,                    Not Modified)                    \
    XX(305, USE_PROXY,                       Use Proxy)                       \
    XX(307, TEMPORARY_REDIRECT,              Temporary Redirect)              \
    XX(308, PERMANENT_REDIRECT,              Permanent Redirect)              \
    XX(400, BAD_REQUEST,                     Bad Request)                     \
    XX(401, UNAUTHORIZED,                    Unauthorized)                    \
    XX(402, PAYMENT_REQUIRED,                Payment Required)                \
    XX(403, FORBIDDEN,                       Forbidden)                       \
    XX(404, NOT_FOUND,                       Not Found)                       \
    XX(405, METHOD_NOT_ALLOWED,              Method Not Allowed)              \
    XX(406, NOT_ACCEPTABLE,                  Not Acceptable)                  \
    XX(407, PROXY_AUTHENTICATION_REQUIRED,   Proxy Authentication Required)   \
    XX(408, REQUEST_TIMEOUT,                 Request Timeout)                 \
    XX(409, CONFLICT,                        Conflict)                        \
    XX(410, GONE,                            Gone)                            \
    XX(411, LENGTH_REQUIRED,                 Length Required)                 \
    XX(412, PRECONDITION_FAILED,             Precondition Failed)             \
    XX(413, PAYLOAD_TOO_LARGE,               Payload Too Large)               \
    XX(414, URI_TOO_LONG,                    URI Too Long)                    \
    XX(415, UNSUPPORTED_MEDIA_TYPE,          Unsupported Media Type)          \
    XX(416, RANGE_NOT_SATISFIABLE,           Range Not Satisfiable)           \
    XX(417, EXPECTATION_FAILED,              Expectation Failed)              \
    XX(421, MISDIRECTED_REQUEST,             Misdirected Request)             \
    XX(422, UNPROCESSABLE_ENTITY,            Unprocessable Entity)            \
    XX(423, LOCKED,                          Locked)                          \
    XX(424, FAILED_DEPENDENCY,               Failed Dependency)               \
    XX(426, UPGRADE_REQUIRED,                Upgrade Required)                \
    XX(428, PRECONDITION_REQUIRED,           Precondition Required)           \
    XX(429, TOO_MANY_REQUESTS,               Too Many Requests)               \
    XX(431, REQUEST_HEADER_FIELDS_TOO_LARGE, Request Header Fields Too Large) \
    XX(451, UNAVAILABLE_FOR_LEGAL_REASONS,   Unavailable For Legal Reasons)   \
    XX(500, INTERNAL_SERVER_ERROR,           Internal Server Error)           \
    XX(501, NOT_IMPLEMENTED,                 Not Implemented)                 \
    XX(502, BAD_GATEWAY,                     Bad Gateway)                     \
    XX(503, SERVICE_UNAVAILABLE,             Service Unavailable)             \
    XX(504, GATEWAY_TIMEOUT,                 Gateway Timeout)                 \
    XX(505, HTTP_VERSION_NOT_SUPPORTED,      HTTP Version Not Supported)      \
    XX(506, VARIANT_ALSO_NEGOTIATES,         Variant Also Negotiates)         \
    XX(507, INSUFFICIENT_STORAGE,            Insufficient Storage)            \
    XX(508, LOOP_DETECTED,                   Loop Detected)                   \
    XX(510, NOT_EXTENDED,                    Not Extended)                    \
    XX(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required) \

/// @brief HTTP请求方法枚举
enum class HTTPMethod{
#define XX(num, name, string) name = num,
    HTTP_METHOD_MAP(XX)
#undef XX
    INVALID_METHOD
};

/// @brief HTTP状态枚举
enum class HTTPStatus{
#define XX(code, name, desc) name = code,
    HTTP_STATUS_MAP(XX)
#undef XX
};

/// @brief 将字符串转为HTTP方法枚举类
/// @param str HTTP方法字符串
/// @return HTTP方法枚举
HTTPMethod StringToHTTPMethod(const std::string& str);

/// @brief 将C风格的字符串转为HTTP方法枚举类型
/// @param str HTTP方法字符串
/// @return HTTP方法枚举
HTTPMethod StringToHTTPMethod(const char* str);

/// @brief 将HTTP方法枚举类型转为字符串
/// @param method HTTP方法枚举
/// @return HTTP方法字符串
const char* HTTPMethodToString(const HTTPMethod& method);

/// @brief 将HTTP状态码枚举转为字符串
/// @param status HTTP状态码
/// @return HTTP状态码字符串
const char* HTTPStatusToString(const HTTPStatus& status);

/// @brief 提供字符串的比较功能的结构体（忽略大小写）
struct StringComparator{
    bool operator()(const std::string& lhs, const std::string& rhs) const;
};

/// @brief 获取Map中的Key对应Value的值，转换成正确的类型并返回是否转换成功
/// @tparam MapType Map数据结构的类型
/// @tparam ValueType 值的类型
/// @param map Map数据结构
/// @param key 键
/// @param value 保存后的值
/// @return 是否转换成功
template<class MapType, class ValueType>
bool getAsCheck(const MapType& map, const std::string& key, ValueType& value, const ValueType& def = ValueType()){
    auto it = map.find(key);
    if(it == map.end()){
        value = def;
        return false;
    }

    try{
        value = boost::lexical_cast<ValueType>(it->second);
        return true;
    }catch( ... ){
        value = def;
    }
    return false;
}

/// @brief 获取Map中的Key对应的Value值
/// @tparam MapType Map数据结构类型
/// @tparam ValueType 值的类型
/// @param map Map数据结构
/// @param key 键
/// @param def 默认值
/// @return 如果键值对存在则返回正确类型的值，否则返回默认值
template<class MapType, class ValueType>
ValueType getAs(const MapType& map, const std::string& key, const ValueType& def = ValueType()){
    auto it = map.find(key);
    if(it == map.end()){
        return def;
    }
    try{
        return boost::lexical_cast<ValueType>(it->second);
    }catch( ... ){
    }
    return def;
}

class HTTPResponse;

/// @brief HTTP请求封装类
class HTTPRequest{
public:
    using ptr = std::shared_ptr<HTTPRequest>;
    using MapType = std::map<std::string, std::string, StringComparator>;

    /// @brief 构造函数
    /// @param version HTTP版本 0x11 = HTTP 1.1
    /// @param close 是否自动关闭
    HTTPRequest(uint8_t version = 0x11, bool auto_close = true);

    /// @brief 由当前请求生成回应
    std::shared_ptr<HTTPResponse> createResponse();

    /// @brief 获取HTTP方法
    HTTPMethod getMethod() const { return m_method; }

    /// @brief 获取HTTP版本
    uint8_t getVersion() const { return m_version; }

    /// @brief 返回请求路径 
    const std::string& getPath() const { return m_path; }

    /// @brief 返回请求参数 
    const std::string& getQuery() const { return m_query; }

    /// @brief 返回请求消息体 
    const std::string& getBody() const { return m_body; }

    /// @brief 返回请求头部键值映射表 
    const MapType& getHeaders() const { return m_headers; }

    /// @brief 返回请求参数键值映射表 
    const MapType& getParams() const { return m_params; }

    /// @brief 返回Cookies的键值映射表 
    const MapType& getCookies() const { return m_cookies; }

    /// @brief 设置HTTP请求方法
    void setMethod(HTTPMethod method) { m_method = method; }

    /// @brief 设置HTTP请求版本 
    void setVersion(uint8_t version) { m_version = version; }

    /// @brief 设置HTTP请求路径 
    void setPath(const std::string& path) { m_path = path; }

    /// @brief 设置HTTP请求路径 
    void setQuery(const std::string& query) { m_query = query; }

    /// @brief 设置HTTP请求的fragment 
    void setFrgment(const std::string& fragment) { m_fragment = fragment; }

    /// @brief 设置HTTP请求的消息体 
    void setBody(const std::string& body) { m_body = body; }

    /// @brief 返回是否自动关闭 
    bool isAutoClose() const { return m_autoClose;}

    /// @brief 设置是否自动关闭 
    void setAutoClose(bool autoClose) { m_autoClose = autoClose; }

    /// @brief 返回是否是websocket
    bool isWebsocket() const { return m_isWebsocket; }
    
    /// @brief 设置是否是websocket 
    void setWebsocket(bool isWebsocket) { m_isWebsocket = isWebsocket; } 

    /// @brief 设置HTTP请求头部键值映射表 
    void setHeaders(const MapType& headers) { m_headers = headers; }

    /// @brief 设置HTTP请求参数键值映射表 
    void setParams(const MapType& params) { m_params = params; }

    /// @brief 设置HTTP请求Cookies的键值映射表
    void setCookies(const MapType& cookies) { m_cookies = cookies; }

    /// @brief 获取HTTP请求的头部参数
    /// @param key 键
    /// @param def 默认值
    /// @return 如果对应键存在则返回对应值，不存在则返回默认值
    std::string getHeader(const std::string& key, const std::string& def = "") const;
    
    /// @brief 获取HTTP请求的请求参数
    /// @param key 键
    /// @param def 默认值
    /// @return 如果存在则返回默认值，不存在则返回默认值
    std::string getParam(const std::string& key, const std::string& def = "");

    /// @brief 获取HTTP请求的Cookies参数
    /// @param key 键
    /// @param def 默认值
    /// @return 如果存在则返回默认值，不存在则返回默认值
    std::string getCookies(const std::string& key, const std::string& def = "");

    /// @brief 设置HTTP请求的头部参数
    /// @param key 键
    /// @param value 值
    void setHeader(const std::string& key, const std::string& value);

    /// @brief 设置HTTP请求的请求参数
    /// @param key 键
    /// @param value 值
    void setParam(const std::string& key, const std::string& value);

    /// @brief 设置HTTP请求的Cookies参数
    /// @param key 键
    /// @param value 值
    void setCookies(const std::string& key, const std::string& value);

    /// @brief 删除HTTP请求的头部参数
    /// @param key 键
    void delHeader(const std::string& key);

    /// @brief 删除HTTP请求的请求参数
    /// @param key 键
    void delParam(const std::string& key);

    /// @brief 删除HTTP请求的Cookies参数
    /// @param key 键
    void delCookies(const std::string& key);

    /// @brief 判断HTTP请求的头部参数是否存在
    /// @param key 键
    /// @param val 如果键存在，val非空
    /// @return 是否存在
    bool hasHeader(const std::string& key, std::string* val = nullptr);
    
    /// @brief 判断HTTP请求的请求参数是否存在
    /// @param key 键
    /// @param val 如果键存在,val非空
    /// @return 是否存在
    bool hasParam(const std::string& key, std::string* val = nullptr);

    /// @brief 判断HTTP请求的Cookies参数是否存在
    /// @param key 键
    /// @param val 如果键存在，val非空
    /// @return 是否存在
    bool hasCookies(const std::string& key, std::string* val = nullptr);

    /// @brief 检查并获取HTTP请求的头部参数
    /// @tparam T 值具体类型
    /// @param key 键
    /// @param value 返回值
    /// @param def 默认值
    /// @return 如果键存在并且值转换成功则返回真
    template<class T>
    bool getHeaderAsCheck(const std::string& key, T& value, const T& def = T()){
        return getAsCheck(m_headers, key, value, def);
    }

    /// @brief 获取HTTP请求的头部参数
    /// @tparam T 值具体类型
    /// @param key 键
    /// @param def 默认值
    /// @return 值
    template<class T>
    T getHeaderAs(const std::string& key, const T& def = T()){
        return getAs(m_headers, key, def);
    }

    /// @brief 检查并获取HTTP请求的请求参数
    /// @tparam T 值具体类型
    /// @param key 键
    /// @param value 值
    /// @param def 默认值
    /// @return 如果键存在且转换为成功则返回真
    template<class T>
    bool getParamAsCheck(const std::string& key, T& value, const T& def = T()){
        initQueryParam();
        initBodyParam();
        return getAsCheck(m_params, key, value, def);
    }

    /// @brief 获取HTTP请求的请求参数
    /// @tparam T 值具体类型
    /// @param key 键
    /// @param def 默认值
    /// @return 值
    template<class T>
    T getParamAs(const std::string& key, const T& def = T()){
        initQueryParam();
        initBodyParam();
        return getAs(m_params, key, def);
    }

    /// @brief 检查并获取HTTP请求的Cookies参数
    /// @tparam T 值具体类型
    /// @param key 键
    /// @param val 值
    /// @param def 默认值
    /// @return 如果键存在并且转换成功则返回真
    template<class T>
    bool getCookiesAsCheck(const std::string& key, T& val, const T& def = T()){
        initCookies();
        return getAsCheck(m_cookies, key, val, def);
    }

    /// @brief 获取HTTP请求的Cookies参数
    /// @tparam T 值具体类型
    /// @param key 键
    /// @param def 默认值
    /// @return 值
    template<class T>
    T getCookiesAs(const std::string& key, const T& def = T()){
        initCookies();
        return getAs(m_cookies, key, def);
    }


    /// @brief 将HTTP请求内容序列化输出到流中
    /// @param os 目标输出流
    /// @return 目标输出流
    std::ostream& dump(std::ostream& os) const;

    /// @brief 将HTTP请求内容转为字符串输出
    std::string toString() const;

    /// @brief 初始化请求体
    void init();
    
    /// @brief 初始化参数
    void initParam();

    /// @brief 初始化请求参数
    void initQueryParam();

    /// @brief 初始化body参数
    void initBodyParam();

    /// @brief 初始化cookies
    void initCookies();
private:
    // HTTP方法
    HTTPMethod m_method;
    // HTTP版本 0x11 = 1.1
    uint8_t m_version;
    // 是否自动关闭
    bool m_autoClose;
    // 是否为websocket
    bool m_isWebsocket;


    uint8_t m_parserParamFlag;
    // 请求路径
    std::string m_path;
    // 请求参数
    std::string m_query;
    // 请求fragment
    std::string m_fragment;
    // 请求消息体
    std::string m_body;
    // 请求头部Map
    MapType m_headers;
    // 请求参数Map
    MapType m_params;
    // 请求Cookies Map
    MapType m_cookies;
};

/// @brief HTTP相应封装类
class HTTPResponse{
public:
    using ptr = std::shared_ptr<HTTPResponse>;
    using MapType = std::map<std::string, std::string, StringComparator>;

    /// @brief 构造函数
    /// @param version 版本
    /// @param auto_close 是否自动关闭
    HTTPResponse(uint8_t version = 0x11, bool auto_close = true);

    /// @brief 获取响应状态
    HTTPStatus getStatus() const { return m_status; }

    /// @brief 获取版本 
    uint8_t getVersion() const { return m_version; }
    
    /// @brief 获取响应消息体
    const std::string& getBody() const { return m_body; }
    
    /// @brief 获取相应原因 
    const std::string& getReasons() const { return m_reasons; }

    /// @brief 获取响应体头部键值映射表 
    const MapType& getHeaders() const { return m_headers; }

    /// @brief 设置响应状态
    void setStatus(HTTPStatus status) { m_status = status; }
    
    /// @brief 设置版本 
    void setVersion(uint8_t version) { m_version = version; }
    
    /// @brief 设置响应消息体
    void setBody(const std::string& body) { m_body = body; }

    /// @brief 设置响应原因 
    void setReasons(const std::string& reasons) { m_reasons = reasons; }

    /// @brief 设置响应体头部键值映射表 
    void setHeaders(const MapType& headers) { m_headers = headers; }

    /// @brief 获取是否自动关闭 
    bool isAutoClose() const { return m_autoClose; }

    /// @brief 设置是否自动关闭 
    void setAutoClose(bool auto_close) { m_autoClose = auto_close; } 

    /// @brief 获取是否是websocket 
    bool isWebsocket() const { return m_isWebsocket; }

    /// @brief 设置是否是Websocket
    void setWebsocket(bool is_websocket) { m_isWebsocket = is_websocket;}

    /// @brief 获取响应体头部参数
    /// @param key 键
    /// @param def 默认值
    /// @return 返回值
    std::string getHeader(const std::string& key, const std::string& def = "") const;

    /// @brief 设置响应体头部参数
    /// @param key 键
    /// @param val 值
    void setHeader(const std::string& key, const std::string& val);

    /// @brief 删除响应体头部参数
    /// @param key 键
    void delHeader(const std::string& key);

    /// @brief 检查并获取相应头部参数
    /// @tparam T 值类型
    /// @param key 键
    /// @param val 值
    /// @param def 默认值
    /// @return 如果键存在并且转换成功则返回真
    template<class T>
    bool getHeaderAsCheck(const std::string& key, T& val, const T& def = T()){
        return getAsCheck(m_headers, key, val, def);
    }

    /// @brief 获取响应体头部参数
    /// @tparam T 值类型
    /// @param key 键
    /// @param def 默认值
    /// @return 值
    template<class T>
    T getHeaderAs(const std::string& key, const T& def = T()){
        return getAs(m_headers, key, def);
    }

    void setCookies(const std::string& key, const std::string& val, time_t expired = 0, const std::string& path = "", 
    const std::string& domain = "", bool secure = false);

    /// @brief 将响应体内容序列化输出到流
    /// @param os 目标流
    /// @return 目标流
    std::ostream& dump(std::ostream& os) const;

    /// @brief 将响应体内容转换成字符串
    std::string toString() const;

private:
    // HTTP相应状态
    HTTPStatus m_status;
    // HTTP版本
    uint8_t m_version;
    // 是否自动关闭
    bool m_autoClose;
    // 是否是websocket
    bool m_isWebsocket;

    // 消息响应体
    std::string m_body;
    // 相应原因
    std::string m_reasons;
    // 相应头部
    MapType m_headers;
    // cookies
    std::vector<std::string> m_cookies;
};  

/// @brief 流式输出HTTP请求内容
/// @param os 目标输出流
/// @param request HTTP请求体
/// @return 目标输出流
std::ostream& operator<<(std::ostream& os, const HTTPRequest& request);

/// @brief 流式输出HTTP相应内容
/// @param os 目标输出流
/// @param response HTTP响应体
/// @return 目标输出流
std::ostream& operator<<(std::ostream& os, const HTTPResponse& response);


}
}

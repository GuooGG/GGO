/**
 * @file httpConnection.h
 * @author GGo
 * @brief HTTP客户端方法封装
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "http/http.h"
#include "streams/socketStream.h"
#include "uri.h"
#include "thread.h"
#include <list>

namespace GGo{
namespace HTTP{

/// @brief HTTP响应结果
struct HTTPResult{
    using ptr = std::shared_ptr<HTTPResult>;

    /// @brief 错误码枚举类定义
    enum class Error{
        /// @brief 正常
        OK = 0,
        /// @brief 非法URL
        INVALID_URL = 1,
        /// @brief 无法解析HOST
        INVALID_HOST = 2,
        /// @brief 连接失败
        CONNECTION_FAIL = 3,
        /// @brief 连接被对方关闭
        SEND_CLOSE_BY_PEER = 4,
        /// @brief 发生socket错误
        SEND_SOCKET_ERROR = 5,
        /// @brief 超时错误
        TIMEOUT = 6,
        /// @brief 创建socket失败
        CREATE_SOCKET_ERROR = 7,
        /// @brief 从连接池种获取连接失败
        POOL_GET_CONNECTION = 8,
        /// @brief 无效的连接
        POOL_INVALID_CONNECTION = 9,
    };

    HTTPResult(int _result
                ,HTTPResponse::ptr _response
                ,const std::string& _error)
                :result(_result)
                ,response(_response)
                ,error(_error){
                    
                }

    // 错误码
    int result;
    // HTTP响应对象
    HTTPResponse::ptr response;
    // 错误描述
    std::string error;

    std::string toString() const;

};

/// @brief HTTP连接池类
class HTTPConnectionPool;

class HTTPConnection : public SocketStream{
friend class HTTPConnectionPool;
public:
    using ptr = std::shared_ptr<HTTPConnection>;

    /// @brief 发送HTTP GET请求
    /// @param url 请求的url字符串
    /// @param timeout_ms 超时时间
    /// @param headers HTTP请求头部参数
    /// @param body 请求消息体
    /// @return HTTP结果结构体
    static HTTPResult::ptr sendGet(const std::string& url, uint64_t timeout_ms 
                                    , const std::map<std::string, std::string>& headers = {}
                                    , const std::string& body = "");
    
    /// @brief 发送HTTP GET请求
    /// @param uri URL对象
    /// @param timeout_ms 超时时间
    /// @param headers HTTP请求头部参数
    /// @param body 请求消息体
    /// @return HTTP结果结构体
    static HTTPResult::ptr sendGet(Uri::ptr url
                                , uint64_t timeout_ms
                                , const std::map<std::string, std::string>& headers = {}
                                , const std::string& body = "");

    /// @brief 发送HTTP POST请求
    /// @param url 请求的url字符串
    /// @param timeout_ms 超时时间
    /// @param headers HTTP请求头部参数
    /// @param body 请求消息体
    /// @return HTTP结果结构体
    static HTTPResult::ptr sendPost(const std::string& url
                                , uint64_t timeout_ms
                                , const std::map<std::string, std::string>& headers = {}
                                , const std::string& body = "");

    /// @brief 发送HTTP POST请求
    /// @param url URL对象
    /// @param timeout_ms 超时时间
    /// @param headers HTTP请求头部参数
    /// @param body 请求消息体
    /// @return HTTP结果结构体
    static HTTPResult::ptr sendPost(Uri::ptr url
                                , uint64_t timeout_ms
                                , const std::map<std::string, std::string>& headers = {}
                                , const std::string& body = "");   

    /// @brief 发送HTTP请求
    /// @param method 请求类型
    /// @param uri 请求的URI字符串
    /// @param timeout_ms 超时时间
    /// @param headers HTTP请求头部参数
    /// @param body 请求消息体
    /// @return HTTP结果结构体
    static HTTPResult::ptr sendRequest(HTTPMethod method
                                , const std::string& url            
                                , uint64_t timeout_ms
                                , const std::map<std::string, std::string>& headers = {}
                                , const std::string& body = "");
    
    /// @brief 发送HTTP请求
    /// @param method 请求类型
    /// @param uri URI对象
    /// @param timeout_ms 超时时间
    /// @param headers 请求头部参数
    /// @param body 请求消息体
    /// @return HTTP结果结构体
    static HTTPResult::ptr sendRequest(HTTPMethod method
                                , Uri::ptr url
                                , uint64_t timeout_ms
                                , const std::map<std::string, std::string>& headers = {}
                                , const std::string& body = "");
    /// @brief 发送HTTP请求
    /// @param request 请求结构体
    /// @param uri URI对象
    /// @param timeout 超时时间
    /// @return HTTP结果结构体
    static HTTPResult::ptr sendRequest(HTTPRequest::ptr request
                                , Uri::ptr url
                                , uint64_t timeout);  

    /// @brief 构造函数
    /// @param sock socket对象
    /// @param owner 是否对socket拥有所有权
    HTTPConnection(Socket::ptr sock, bool owner = true);

    /// @brief 析构函数
    ~HTTPConnection();

    /// @brief 接收HTTP响应
    HTTPResponse::ptr recvResponse();

    /// @brief 发送HTTP请求
    /// @param request HTTP请求结构体
    int sendRequest(HTTPRequest::ptr request);

private:
    uint64_t m_createTime = 0;
    uint64_t m_request = 0;
};

class HTTPConnectionPool{

};


} 
}

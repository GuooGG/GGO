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


};

class HTTPConnectionPool{

};


} 
}

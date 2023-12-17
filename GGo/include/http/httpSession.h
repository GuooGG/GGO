/**
 * @file httpSession.h
 * @author GGo
 * @brief httpSession流式封装
 * @date 2023-12-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "streams/socketStream.h"
#include "http/http.h"

namespace GGo{
namespace HTTP{

class HTTPSession : public SocketStream{
public:
    using ptr = std::shared_ptr<HTTPSession>;

    /// @brief 构造函数
    /// @param socket socket对象
    /// @param isOwner 是否为该socket的主要管理者
    HTTPSession(Socket::ptr socket, bool isOwner = true);

    /// @brief 接收HTTP请求
    HTTPRequest::ptr recvRequest();

    /// @brief 发送HTTP响应
    /// @param response HTTP响应对象
    /// @return 
    ///         @retval >0 发送成功
    ///         @retval =0 对方关闭
    ///         @retval <0 发生异常
    int sendResponse(HTTPResponse::ptr response);
};

}
}


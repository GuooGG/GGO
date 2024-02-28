/**
 * @file httpSever.h
 * @author GGo
 * @brief HTTP服务器封装
 * @version 0.1
 * @date 2024-01-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "TCPSever.h"
#include "http/httpSession.h"
#include "http/servlet.h"


namespace GGo{
namespace HTTP{

class HTTPSever : public TCPSever{
public:
    using prt = std::shared_ptr<HTTPSever>;
    
    /// @brief 构造函数
    /// @param keppalive 是否支持长连接 
    /// @param worker 工作调度器
    /// @param accept_worker 接受连接调度器
    HTTPSever(bool keppalive = false,
                GGo::IOScheduler* worker = GGo::IOScheduler::getThis(),
                GGo::IOScheduler* io_worker = GGo::IOScheduler::getThis(),
                GGo::IOScheduler* accept_worker = GGo::IOScheduler::getThis());
    
    /// @brief 获取servlet分发器
    ServletDispatch::ptr getServletDispatch() const { return m_dispatch;}

    /// @brief 设置servlet分发器 
    void setSevletDispatch(ServletDispatch::ptr v) { m_dispatch = v; }

    virtual void setName(const std::string& name) override;

protected:

    virtual void handleCilent(Socket::ptr cilent) override;

private:
    // 是否为长连接
    bool m_isKeepAlive;
    // servlet分发器
    ServletDispatch::ptr m_dispatch;
};


}
}
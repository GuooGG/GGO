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

namespace GGo{
namespace HTTP{

class HTTPSever : public TCPSever{
public:
    using prt = std::shared_ptr<HTTPSever>;
    
private:
    // 是否为长连接
    bool m_isKeepAlive;
    
};


}
}
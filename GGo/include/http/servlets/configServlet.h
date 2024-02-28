/**
 * @file configServlet.h
 * @author GGo
 * @version 0.1
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "http/servlet.h"

namespace GGo{
namespace HTTP{

class ConfigServlet :public servlet{
public:
    ConfigServlet();
    virtual int32_t handle(GGo::HTTP::HTTPRequest::ptr request
                        , GGo::HTTP::HTTPResponse::ptr response
                        , GGo::HTTP::HTTPSession::ptr session) override;
};

}
}
#include "http/servlets/configServlet.h"
#include "config.h"


namespace GGo{
namespace HTTP{

ConfigServlet::ConfigServlet()
        :servlet("ConfigServlet")
{

}

int32_t ConfigServlet::handle(GGo::HTTP::HTTPRequest::ptr request,
                                GGo::HTTP::HTTPResponse::ptr response, 
                                GGo::HTTP::HTTPSession::ptr session)
{
    std::string type = request->getParam("type");
    if("json" == type){
        response->setHeader("Content-Type", "text/json charset=utf-8");
    }else{
        response->setHeader("Content-Type", "text/yaml charset=utf-8");
    }
    YAML::Node node;
    GGo::Config::visit([&node](ConfigVarBase::ptr base){
        YAML::Node n;
        try{
            n = YAML::Load(base->toString());
        }catch(...){
            return;
        }
        node[base->getName()] = n;
        node[base->getName() + "$description"] = base->getDescription();
    });

    if(type == "json"){
        //TODO::处理JSON的情况
    }
    std::stringstream ss;
    ss << node;
    response->setBody(ss.str());
    return 0;
}
}
}


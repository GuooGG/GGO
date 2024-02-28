#include "http/servlets/statusServlet.h"

namespace GGo{
namespace HTTP{


statusServlet::statusServlet()
    :servlet("statusServlet")
{

}
int32_t statusServlet::handle(GGo::HTTP::HTTPRequest::ptr request, 
                                GGo::HTTP::HTTPResponse::ptr response, 
                                GGo::HTTP::HTTPSession::ptr session)
{
    return 0;
}


}

}
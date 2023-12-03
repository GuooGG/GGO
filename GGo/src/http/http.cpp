#include "http/http.h"

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
}
}






